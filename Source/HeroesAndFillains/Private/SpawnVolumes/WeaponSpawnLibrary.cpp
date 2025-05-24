// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolumes/WeaponSpawnLibrary.h"
#include "Weapons/Ranged/ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AProjectileWeapon* UWeaponSpawnLibrary::SpawnFloatingWeapon(UObject* WorldContextObject, TSubclassOf<AProjectileWeapon> WeaponClass, FVector SpawnLocation, FRotator SpawnRotation)
{
    if (!WeaponClass || !WorldContextObject) return nullptr;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AProjectileWeapon* SpawnedWeapon = World->SpawnActor<AProjectileWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedWeapon)
    {
        SpawnedWeapon->bShouldHover = true;
        SpawnedWeapon->bShouldFloatSpin = true;

        if (SpawnedWeapon->AreaSphere)
        {
            SpawnedWeapon->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            SpawnedWeapon->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
            SpawnedWeapon->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
            SpawnedWeapon->AreaSphere->SetGenerateOverlapEvents(true);
        }
    }

    return SpawnedWeapon;
}