// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/HAFFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Components/AudioComponent.h"


void AHAFFireball::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AHAFFireball::OnNewSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// UE_LOG(LogTemp, Warning, TEXT("✅ Target %s has ASC"), *GetNameSafe(OtherActor));
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UHAFAbilitySystemBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AHAFFireball::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FHAFGameplayTags::Get().GameplayCue_Fireblast, CueParams);

		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
			LoopingSoundComponent->DestroyComponent();
		}
		bHit = true;
	}
}
