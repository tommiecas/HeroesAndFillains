// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFProjectileFiringAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "Weapons/Ranged/HAFRangedProjectileBullet.h"


void UHAFProjectileFiringAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Only the server actually spawns the authoritative projectile,
	// but clients should still see a predicted version.
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client predicting projectile bullet spawn"));
	}
	UE_LOG(LogTemp, Warning, TEXT("🔥 Bullet Ability Activated!"));

	// if (IsActive())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("FireBolt already active — skipping duplicate activation"));
	// 	return;
	// }
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

FGameplayEffectContextHandle UHAFProjectileFiringAbility::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context,
	UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	NewContext.AddSourceObject(SourceObject);
	return NewContext;
}

void UHAFProjectileFiringAbility::SpawnProjectileBullet(const FVector& ProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	if (GetAvatarActorFromActorInfo()->ActorHasTag(FName("Enemy")))
	{
		/*	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
				GetAvatarActorFromActorInfo(),
				FHAFGameplayTags::Get().Montage_Attack_Weapon);
			const FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rotation.Quaternion());
	
			if (!HAFRangedProjectileBulletClass) return;
	
			AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
				HAFRangedProjectileBulletClass,
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
		{ */
		FName SocketName = "FiringSocket";
		USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
		if (!Mesh || !Mesh->DoesSocketExist(SocketName)) return;

		const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);

		// ✅ Adjust target Z height — aim toward the middle of the player
		FVector AdjustedTarget = ProjectileTargetLocation;
		AdjustedTarget.Z += 60.f; // tune this for your character height

		// ✅ Compute the correct firing rotation
		const FVector AimDirection = (AdjustedTarget - SocketLocation).GetSafeNormal();
		const FRotator AimRotation = AimDirection.Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(AimRotation.Quaternion());

		if (!HAFRangedProjectileBulletClass) return;

		AHAFRangedProjectileBullet* ProjectileBullet = GetWorld()->SpawnActorDeferred<AHAFRangedProjectileBullet>(
			HAFRangedProjectileBulletClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(ProjectileBullet);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(ProjectileBullet);
		// ✅ FIX: Add the shooter (enemy) as the source actor, not the player target
		Actors.Add(GetAvatarActorFromActorInfo());
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = AdjustedTarget;
		EffectContextHandle.AddHitResult(HitResult);
		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		
		ProjectileBullet->BulletDamageEffectSpecHandle = SpecHandle;
		
		ProjectileBullet->FinishSpawning(SpawnTransform);

		// Debug line to verify aim
		DrawDebugLine(GetWorld(), SocketLocation, SocketLocation + AimDirection * 2000.f, FColor::Red, false, 2.f, 0, 1.f);
	}
}
