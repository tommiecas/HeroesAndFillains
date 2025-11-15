// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/WeaponBase.h"
#include "Characters/FillainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "GameFramework/Pawn.h"
#include "HAFComponents/LagCompensationComponent.h"
#include "PlayerController/FillainPlayerController.h"
#include "Interfaces/HitInterface.h"


void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn == nullptr) return;
	AController* InstigatingController = OwningPawn->GetController();

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleSocket && InstigatingController)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(FireHit.GetActor());
		if (FillainCharacter && HasAuthority() && InstigatingController)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwningPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				// Damage is now handled through GAS via GetHit_Implementation
				// which will apply the appropriate GameplayEffect
				if (IHitInterface* HitInterface = Cast<IHitInterface>(FillainCharacter))
				{
					HitInterface->Execute_GetHit(FillainCharacter, FireHit.ImpactPoint, GetOwner());
				}
			}
			if (!HasAuthority() && bUseServerSideRewind)
			{
				FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(Owner) : FillainOwnerCharacter;
				FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(InstigatingController) : FillainOwnerController;
				if (FillainOwnerController && FillainOwnerCharacter && FillainOwnerCharacter->GetLagCompensation() && FillainOwnerCharacter->IsLocallyControlled())
				{
					FillainOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						FillainCharacter,
						Start,
						HitTarget,
						FillainOwnerController->GetServerTime() - FillainOwnerController->SingleTripTime
					);
				}
			}
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
				FireHit.ImpactPoint
				);
		}
		if (MuzzleSocket)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
	}
}


void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector TraceEnd = bUseScatter ? TraceEndWithScatter(HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			TraceEnd,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = TraceEnd;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = TraceEnd;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

