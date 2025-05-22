// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponsFinal/Ranged/ProjectileWeapon.h"
#include "WeaponSpawnLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UWeaponSpawnLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Spawning")
	static AProjectileWeapon* SpawnFloatingWeapon(UObject* WorldContextObject, TSubclassOf<AProjectileWeapon> WeaponClass, FVector SpawnLocation, FRotator SpawnRotation);
	
};
