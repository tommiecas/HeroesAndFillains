// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/Shotgun.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/WeaponBase.h"
#include "Characters/FillainCharacter.h"
#include "HAFComponents/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerController/FillainPlayerController.h"
#include "Sound/SoundCue.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Interfaces/HitInterface.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	ARangedWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetRangedWeaponMesh()->GetSocketByName("MuzzleFlashSocket");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetRangedWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to number of times hit
		TMap<AFillainCharacter*, uint32> HitMap;
		TMap<AFillainCharacter*, uint32> HeadShotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(FireHit.GetActor());
			if (FillainCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(FillainCharacter)) 
						HeadShotHitMap[FillainCharacter]++;
					else 
						HeadShotHitMap.Emplace(FillainCharacter, 1); // Fixed: Use HeadShotHitMap
				}


				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						ImpactParticles,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						HitSound,
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f, .5f)
					);
				}
			}
		}
		TArray<AFillainCharacter*>  HitCharacters;
		TMap<AFillainCharacter*, float> DamageMap;
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// Calculate head shot damage by multiplying times hit x HeadShotDamage - store in DamageMap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		// Loop through DamageMap to apply damage to each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					// Damage is now handled through GAS via GetHit_Implementation
					// which will apply the appropriate GameplayEffect
					// Note: For shotgun, we still track multiple hits but GAS will handle the actual damage
					if (IHitInterface* HitInterface = Cast<IHitInterface>(DamagePair.Key))
					{
						// Use the first hit location from HitTargets as impact point
						FVector ImpactPoint = HitTargets.Num() > 0 ? HitTargets[0] : FVector::ZeroVector;
						HitInterface->Execute_GetHit(DamagePair.Key, ImpactPoint, GetOwner());
					}
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(OwnerPawn) : FillainOwnerCharacter;
			FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(InstigatorController) : FillainOwnerController;
			if (FillainOwnerController && FillainOwnerCharacter && FillainOwnerCharacter->GetLagCompensation() && FillainOwnerCharacter->IsLocallyControlled())
			{
				FillainOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					FillainOwnerController->GetServerTime() - FillainOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	if (!IsValid(GetRangedWeaponMesh())) return;
    
	const USkeletalMeshSocket* MuzzleFlashSocket = GetRangedWeaponMesh()->GetSocketByName("MuzzleFlashSocket");
	if (!MuzzleFlashSocket) return;
    
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetRangedWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
    
	FVector ToTarget = HitTarget - TraceStart;
	if (ToTarget.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		ToTarget = GetRangedWeaponMesh()->GetForwardVector() * 100.0f;
	}
    
	const FVector ToTargetNormalized = ToTarget.GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * FMath::Max(1.0f, DistanceToSphere);

	// Pre-allocate array capacity
	HitTargets.Empty(BuckshotCount);
	HitTargets.Reserve(BuckshotCount);

	for (uint32 i = 0; i < BuckshotCount; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
        
		float Size = ToEndLoc.Size();
		if (Size < KINDA_SMALL_NUMBER)
		{
			Size = KINDA_SMALL_NUMBER;
		}
        
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / Size;
		HitTargets.Add(ToEndLoc);
	}
}


		

		
