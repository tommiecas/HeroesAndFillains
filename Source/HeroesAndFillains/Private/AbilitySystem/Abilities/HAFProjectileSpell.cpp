// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "Characters/FillainCharacter.h"


void UHAFProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Only the server actually spawns the authoritative projectile,
	// but clients should still see a predicted version.
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client predicting projectile spawn"));
	}
	UE_LOG(LogTemp, Warning, TEXT("🔥 FireBolt Ability Activated!"));

	// if (IsActive())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("FireBolt already active — skipping duplicate activation"));
	// 	return;
	// }
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

FGameplayEffectContextHandle UHAFProjectileSpell::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context,
	UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	NewContext.AddSourceObject(SourceObject);
	return NewContext;
}

void UHAFProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	if (GetAvatarActorFromActorInfo()->ActorHasTag(FName("Enemy")))
	{
		TArray<FVector> SocketLocations = ICombatInterface::Execute_GetCombatSocketLocations(
			GetAvatarActorFromActorInfo(),
			SocketTag);
		
		// Use the first socket location (or average them for multi-socket attacks)
		const FVector SocketLocation = SocketLocations.Num() > 0 ? SocketLocations[0] : GetAvatarActorFromActorInfo()->GetActorLocation();
		const FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		if (!HAFMajixProjectileClass) return;

		AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
			HAFMajixProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		
		Projectile->DamageEffectSpecHandle = SpecHandle;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	else if (GetAvatarActorFromActorInfo()->ActorHasTag(FName("Player")))
	{
		AFillainCharacter* Fill = Cast<AFillainCharacter>(GetAvatarActorFromActorInfo());
		Fill->EnterCombat();
		
		FName SocketName = "SpellSocket";
		USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
		if (!Mesh || !Mesh->DoesSocketExist(SocketName)) return;
		const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);
		const FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		if (!HAFMajixProjectileClass) return;

		AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
			HAFMajixProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		
		Projectile->DamageEffectSpecHandle = SpecHandle;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}
