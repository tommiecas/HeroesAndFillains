// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "Characters/FillainCharacter.h"
#include "GameMode/HaFGameMode.h"



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
	UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile CALLED. Avatar=%s"),
	*GetAvatarActorFromActorInfo()->GetName());
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar || !HAFMajixProjectileClass) return;

	if (USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh Component Rot: %s"), *Mesh->GetComponentRotation().ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("Character Mesh Rot: %s"), *Avatar->GetActorRotation().ToString());

    const bool bIsEnemy = Avatar->ActorHasTag(FName("Enemy"));
    const bool bIsPlayer = Avatar->ActorHasTag(FName("Player"));

    FVector SocketLocation;
    FRotator SpawnRotation;

    // ----------------------------------------
    // 1) Determine socket location
    // ----------------------------------------

    if (bIsEnemy)
    {
        TArray<FVector> SocketLocations = ICombatInterface::Execute_GetCombatSocketLocations(Avatar, SocketTag);
        SocketLocation = SocketLocations.Num() > 0 ? SocketLocations[0] : Avatar->GetActorLocation();
    }
    else if (bIsPlayer)
    {
        FName SocketName = "SpellSocket";
        USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>();
        if (!Mesh || !Mesh->DoesSocketExist(SocketName)) return;

        SocketLocation = Mesh->GetSocketLocation(SocketName);

        // Player-specific state
        if (AFillainCharacter* Fill = Cast<AFillainCharacter>(Avatar))
        {
            Fill->EnterCombat();
        }
    }

    // ----------------------------------------
    // 2) FIX: Correct aim direction (cursor → camera → hand)
    // ----------------------------------------

	// ----------------------------------------
	// 2) AIM DIRECTION (camera → cursor)
	// ----------------------------------------

	APlayerController* PC = Cast<APlayerController>(Avatar->GetInstigatorController());
	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		// Step 1: Fully align camera XY plane with the hand
		FVector CamLocAdjusted = CamLoc;

		// Match hand HEIGHT
		CamLocAdjusted.Z = SocketLocation.Z;

		// Match left/right alignment
		CamLocAdjusted.Y = SocketLocation.Y;

		// Match forward/back alignment
		CamLocAdjusted.X = SocketLocation.X;

		// Step 2: Build corrected direction
		FVector AimDirection = (ProjectileTargetLocation - CamLocAdjusted).GetSafeNormal();
		SpawnRotation = AimDirection.Rotation();

		SpawnRotation.Yaw -= 16.f;

		UE_LOG(LogTemp, Warning, TEXT("AimDirection FINAL: %s"), *AimDirection.ToString());

		UE_LOG(LogTemp, Warning, TEXT("AimDirection FINAL: %s"), *AimDirection.ToString());

		UE_LOG(LogTemp, Warning, TEXT("AimDirection FIXED: %s"), *AimDirection.ToString());


		UE_LOG(LogTemp, Warning, TEXT("AimDirection: %s"), *AimDirection.ToString());
		UE_LOG(LogTemp, Warning, TEXT("SpawnRotation BEFORE correction: %s"), *SpawnRotation.ToString());
	}

    // ----------------------------------------
    // 3) Create spawn transform
    // ----------------------------------------

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SocketLocation);
    SpawnTransform.SetRotation(SpawnRotation.Quaternion());

    // ----------------------------------------
    // 4) Spawn projectile (keep your deferred spawn + context logic)
    // ----------------------------------------

    AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
        HAFMajixProjectileClass,
        SpawnTransform,
        GetOwningActorFromActorInfo(),
        Cast<APawn>(GetOwningActorFromActorInfo()),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (!Projectile) return;

    // Build GameplayEffectContext
    const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar);
    FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
    EffectContextHandle.SetAbility(this);
    EffectContextHandle.AddSourceObject(Projectile);

    TArray<TWeakObjectPtr<AActor>> Actors;
    Actors.Add(Projectile);
    EffectContextHandle.AddActors(Actors);

    FHitResult HitResult;
    HitResult.Location = ProjectileTargetLocation; 
    EffectContextHandle.AddHitResult(HitResult);

    // Damage GE
    const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

    // Tag-based damage assignment (untouched)
    for (auto& Pair : DamageTypes)
    {
        const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    }

    Projectile->DamageEffectSpecHandle = SpecHandle;

    // ----------------------------------------
    // 5) Finish spawning
    // ----------------------------------------

    Projectile->FinishSpawning(SpawnTransform);

	UE_LOG(LogTemp, Warning, TEXT("Projectile ForwardVector: %s"),
	*Projectile->GetActorForwardVector().ToString());

}



