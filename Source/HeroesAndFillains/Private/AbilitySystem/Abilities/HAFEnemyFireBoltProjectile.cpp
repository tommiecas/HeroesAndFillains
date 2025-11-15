// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFEnemyFireBoltProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "Weapons/Ranged/HAFRangedProjectileBullet.h"


void UHAFEnemyFireBoltProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!GetAvatarActorFromActorInfo())
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbility: Avatar actor is null!"));
		return;
	}

	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client predicting projectile Enemy FireBolt spawn"));
	}
	UE_LOG(LogTemp, Warning, TEXT("🔥 Enemy FireBolt Ability Activated!"));

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

FGameplayEffectContextHandle UHAFEnemyFireBoltProjectile::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context, UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	if (SourceObject)
	{
		NewContext.AddSourceObject(SourceObject);
	}
	return NewContext;
}

void UHAFEnemyFireBoltProjectile::SpawnEnemyFireBoltProjectile(const FVector& EnemyFireBoltProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: Avatar is null!"));
		return;
	}

	if (!Avatar->ActorHasTag(FName("Enemy")))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEnemyFireBoltProjectile: Avatar is not tagged as Enemy"));
		return;
	}

	// Validate projectile class
	if (!HAFMajixProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: HAFMajixProjectileClass is null!"));
		return;
	}

	// Validate damage effect class
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: DamageEffectClass is null!"));
		return;
	}

	// Get mesh and validate socket
	FName SocketName = "SpellSocket";
	USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: No skeletal mesh found on avatar!"));
		return;
	}

	if (!Mesh->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: Socket '%s' does not exist!"), *SocketName.ToString());
		return;
	}

	const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);

	// Adjust target Z height
	FVector AdjustedTarget = EnemyFireBoltProjectileTargetLocation;
	AdjustedTarget.Z += 60.f;

	// Compute firing rotation
	const FVector AimDirection = (AdjustedTarget - SocketLocation).GetSafeNormal();
	const FRotator AimRotation = AimDirection.Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(AimRotation.Quaternion());

	// Spawn projectile
	AHAFMajixProjectile* EnemyFireBoltProjectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
		HAFMajixProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!EnemyFireBoltProjectile)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: Failed to spawn projectile!"));
		return;
	}

	// Get ability system component with validation
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar);
	if (!SourceASC)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: Source ASC is null!"));
		EnemyFireBoltProjectile->Destroy();
		return;
	}

	// Create effect context
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(EnemyFireBoltProjectile);
	
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(TWeakObjectPtr<AActor>(EnemyFireBoltProjectile));
	Actors.Add(TWeakObjectPtr<AActor>(Avatar));
	EffectContextHandle.AddActors(Actors);
	
	FHitResult HitResult;
	HitResult.Location = AdjustedTarget;
	EffectContextHandle.AddHitResult(HitResult);

	// Create damage spec
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnEnemyFireBoltProjectile: Failed to create damage spec!"));
		EnemyFireBoltProjectile->Destroy();
		return;
	}

	// Assign damage magnitudes
	for (const auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	EnemyFireBoltProjectile->DamageEffectSpecHandle = SpecHandle;
	EnemyFireBoltProjectile->FinishSpawning(SpawnTransform);

	// Debug visualization
	DrawDebugLine(GetWorld(), SocketLocation, SocketLocation + AimDirection * 2000.f, FColor::Red, false, 2.f, 0, 1.f);
}
