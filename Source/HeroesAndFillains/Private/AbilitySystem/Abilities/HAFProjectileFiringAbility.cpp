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
	if (!GetAvatarActorFromActorInfo())
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbility: Avatar actor is null!"));
		return;
	}

	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client predicting projectile bullet spawn"));
	}
	UE_LOG(LogTemp, Warning, TEXT("🔥 Bullet Ability Activated!"));

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

FGameplayEffectContextHandle UHAFProjectileFiringAbility::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context, UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	if (SourceObject)
	{
		NewContext.AddSourceObject(SourceObject);
	}
	return NewContext;
}

void UHAFProjectileFiringAbility::SpawnProjectileBullet(const FVector& ProjectileTargetLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: Avatar is null!"));
		return;
	}

	// Validate projectile class
	if (!HAFRangedProjectileBulletClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: HAFRangedProjectileBulletClass is null!"));
		return;
	}

	// Validate damage effect class
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: DamageEffectClass is null!"));
		return;
	}

	// Get mesh and validate socket
	FName SocketName = "FiringSocket";
	USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: No skeletal mesh found on avatar!"));
		return;
	}

	if (!Mesh->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: Socket '%s' does not exist!"), *SocketName.ToString());
		return;
	}

	const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);

	// Adjust target Z height
	FVector AdjustedTarget = ProjectileTargetLocation;
	AdjustedTarget.Z += 60.f;

	// Compute firing rotation
	const FVector AimDirection = (AdjustedTarget - SocketLocation).GetSafeNormal();
	const FRotator AimRotation = AimDirection.Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(AimRotation.Quaternion());

	// Spawn projectile
	AHAFRangedProjectileBullet* ProjectileBullet = GetWorld()->SpawnActorDeferred<AHAFRangedProjectileBullet>(
		HAFRangedProjectileBulletClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!ProjectileBullet)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: Failed to spawn projectile!"));
		return;
	}

	// Get ability system component with validation
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar);
	if (!SourceASC)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: Source ASC is null!"));
		ProjectileBullet->Destroy();
		return;
	}

	// Create effect context
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(ProjectileBullet);
	
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(TWeakObjectPtr<AActor>(ProjectileBullet));
	Actors.Add(TWeakObjectPtr<AActor>(Avatar));
	EffectContextHandle.AddActors(Actors);
	
	FHitResult HitResult;
	HitResult.Location = AdjustedTarget;
	EffectContextHandle.AddHitResult(HitResult);

	// Create damage spec
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnProjectileBullet: Failed to create damage spec!"));
		ProjectileBullet->Destroy();
		return;
	}

	// Assign damage magnitudes
	for (const auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	ProjectileBullet->BulletDamageEffectSpecHandle = SpecHandle;
	ProjectileBullet->FinishSpawning(SpawnTransform);

	// Debug visualization
	DrawDebugLine(GetWorld(), SocketLocation, SocketLocation + AimDirection * 2000.f, FColor::Red, false, 2.f, 0, 1.f);
}
