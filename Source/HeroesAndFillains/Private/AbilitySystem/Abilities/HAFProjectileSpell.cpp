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

void UHAFProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	// UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile CALLED. Avatar=%s"),
	// *GetAvatarActorFromActorInfo()->GetName());
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar || !HAFMajixProjectileClass) return;

	// if (USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Mesh Component Rot: %s"), *Mesh->GetComponentRotation().ToString());
	// }
	// UE_LOG(LogTemp, Warning, TEXT("Character Mesh Rot: %s"), *Avatar->GetActorRotation().ToString());

    const bool bIsEnemy = Avatar->ActorHasTag(FName("Enemy"));
    const bool bIsPlayer = Avatar->ActorHasTag(FName("Player"));

    // FVector SocketLocation;
    // FRotator SpawnRotation;

    // ----------------------------------------
    // 1) Determine socket location
    // THIS IS CHATGPT'S VERSION BELOW----------------------------------------

    /*if (bIsEnemy)
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

	
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);
    Projectile->FinishSpawning(SpawnTransform); */

	//This is the original version from the class.

	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(HAFMajixProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

	Projectile->FinishSpawning(SpawnTransform);



}



