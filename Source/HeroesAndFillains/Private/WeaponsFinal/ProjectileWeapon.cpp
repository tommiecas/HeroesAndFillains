// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/ProjectileFinal.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// from Muzzle Flash Socket to Hit Location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileFinalClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectileFinal>(
					ProjectileFinalClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				); 
			}
		}
	}
}
