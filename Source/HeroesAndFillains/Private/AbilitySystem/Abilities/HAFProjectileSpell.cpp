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

void UHAFProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	const bool bIsServer = Avatar->HasAuthority();
	if (!bIsServer) return;

	if (!HAFMajixProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UHAFProjectileSpell::SpawnProjectile called without HAFMajixProjectileClass set"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UHAFProjectileSpell::SpawnProjectile: World is null"));
		return;
	}
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Avatar);
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetSpellCastersSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AHAFMajixProjectile* Projectile = World->SpawnActorDeferred<AHAFMajixProjectile>(
			HAFMajixProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (!Projectile)
		{
			UE_LOG(LogTemp, Warning, TEXT("UHAFProjectileSpell::SpawnProjectile failed to spawn projectile"));
			return;
		}

		// If you later need to pass effect classes, set them on the projectile before FinishSpawning.
		/* const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();
		const float ScaledDamage = Damage.GetValueAtLevel(10);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
		Projectile->DamageEffectSpecHandle = SpecHandle; */

		Projectile->FinishSpawning(SpawnTransform);
	}
}

