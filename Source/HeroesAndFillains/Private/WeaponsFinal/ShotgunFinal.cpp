// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/ShotgunFinal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "Characters/FillainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgunFinal::Fire(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		TMap<AFillainCharacter*, uint32> HitMap;
		for (uint32 i = 0; i < Buckshot; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(FireHit.GetActor());
			if (FillainCharacter && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(FillainCharacter))
				{
					HitMap[FillainCharacter]++;
				}
				else
				{
					HitMap.Emplace(FillainCharacter, 1);
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
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
					HitPair.Key,
					Damage * HitPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}
		

		
