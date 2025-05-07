// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/HitScanWeaponFinal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "Characters/FillainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"


void AHitScanWeaponFinal::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(FireHit.GetActor());
		if (FillainCharacter && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				FillainCharacter,
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
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
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
	}
}
	


FVector AHitScanWeaponFinal::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = (EndLoc - TraceStart);

	/* DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Green, true);
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()), FColor::Cyan, true); */
	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}

void AHitScanWeaponFinal::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector TraceEnd = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
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

