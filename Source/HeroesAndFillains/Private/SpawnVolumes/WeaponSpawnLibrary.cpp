// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolumes/WeaponSpawnLibrary.h"
#include "Weapons/Ranged/ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

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

        if (SpawnedWeapon->GetSphereCollision())
        {
            SpawnedWeapon->GetSphereCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            SpawnedWeapon->GetSphereCollision()->SetCollisionResponseToAllChannels(ECR_Ignore);
            SpawnedWeapon->GetSphereCollision()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
            SpawnedWeapon->GetSphereCollision()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
            SpawnedWeapon->GetSphereCollision()->SetGenerateOverlapEvents(true);
        }
    }

    return SpawnedWeapon;
}
