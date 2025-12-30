// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFBeamSpell.h"

#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Characters/FillainCharacter.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"


void UHAFBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
		if (FirstActorHit == nullptr) FirstActorHit = MouseHitActor;
		UE_LOG(LogTemp, Warning, TEXT("BlockingHit: %d"), HitResult.bBlockingHit);
		UE_LOG(LogTemp, Warning, TEXT("ImpactPoint: %s"), *HitResult.ImpactPoint.ToString());
		UE_LOG(LogTemp, Warning, TEXT("TraceStart: %s"), *HitResult.TraceStart.ToString());
		UE_LOG(LogTemp, Warning, TEXT("TraceEnd: %s"), *HitResult.TraceEnd.ToString());
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UHAFBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

/*
void UHAFBeamSpell::BuildShockChain(TArray<FShockChainLink>& OutChain)
{
	OutChain.Reset();

	// Safety checks
	if (!OwnerCharacter || !MouseHitActor)
	{
		return;
	}

	// Step 1: Gather additional targets (already sorted by distance)
	TArray<AActor*> AdditionalTargets;
	StoreAdditionalBeamTargets(AdditionalTargets);

	// Step 2: Start chain
	AActor* CurrentSource = OwnerCharacter;
	AActor* CurrentTarget = MouseHitActor;

	// First link: Player → First Guul
	if (CurrentSource != CurrentTarget)
	{
		OutChain.Add({ CurrentSource, CurrentTarget });
	}

	// Step 3: Chain through additional targets
	for (AActor* Candidate : AdditionalTargets)
	{
		if (!Candidate)
		{
			continue;
		}

		// 🚫 Skip self
		if (Candidate == CurrentTarget)
		{
			continue;
		}

		// 🚫 Skip if already used as a target
		const bool bAlreadyChained = OutChain.ContainsByPredicate(
		[Candidate](const FShockChainLink& Link)
		{
			return Link.Target == Candidate;
		});

		if (bAlreadyChained)
		{
			continue;
		}

		// Add link: Previous Target → New Target
		OutChain.Add({ CurrentTarget, Candidate });

		// Advance the chain
		CurrentTarget = Candidate;

		// Respect max chain count
		if (OutChain.Num() >= NumShockTargetsMax)
		{
			break;
		}
	}
}
*/

void UHAFBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Mesh = ICombatInterface::Execute_GetSpellCaster(OwnerCharacter))
		{
			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			if (AFillainCharacter* FC = Cast<AFillainCharacter>(OwnerCharacter))
			{
				const FVector SocketLocation = FC->GetMesh()->GetSocketLocation(FName("SpellSocket"));
				UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
				if (HitResult.bBlockingHit)
				{
					MouseHitLocation = HitResult.ImpactPoint;
					MouseHitActor = HitResult.GetActor();
				}
			}
			if (AEnemyBase* EB = Cast<AEnemyBase>(OwnerCharacter))
			{
				const FVector SocketLocation = EB->GetMesh()->GetSocketLocation(FName("ShockSocket")); 
				UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
				if (HitResult.bBlockingHit)
				{	
					MouseHitLocation = HitResult.ImpactPoint;
					MouseHitActor = HitResult.GetActor();
				}
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UHAFBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UHAFBeamSpell::PrimaryTargetDied);			
		}
	}
}

void UHAFBeamSpell::StoreAdditionalBeamTargets(TArray<AActor*>& OutAdditionalTargets)
{
	if (MouseHitActor == nullptr && FirstActorHit != nullptr) MouseHitActor = FirstActorHit;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	
	TArray<AActor*> OverlappingActors;
	UHAFAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, MouseHitActor->GetActorLocation());
	// int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, NumShockTargetsMax);
	int32 NumAdditionalTargets = 5;

	UHAFAbilitySystemBlueprintLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());	

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UHAFBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UHAFBeamSpell::AdditionalTargetDied);			
			}
		}
	}
}

