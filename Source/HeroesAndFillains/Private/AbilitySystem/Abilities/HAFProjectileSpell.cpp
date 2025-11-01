// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"


void UHAFProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

FGameplayEffectContextHandle UHAFProjectileSpell::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context,
	UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	NewContext.AddSourceObject(SourceObject);
	return NewContext;
}

void UHAFProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetSpellCastersSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
			HAFMajixProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		// ✅ USE THE SPEC HANDLE PASSED FROM BLUEPRINT
		Projectile->DamageEffectSpecHandle = DamageSpecHandle;
        
		// ✅ DEBUG LOG
		UE_LOG(LogTemp, Warning, TEXT("✅ Projectile spawned with DamageSpecHandle. Valid: %d"), 
			DamageSpecHandle.IsValid());
        
		Projectile->FinishSpawning(SpawnTransform);
	}
}