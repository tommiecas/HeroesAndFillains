// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	static AProjectileWeaponFinal* SpawnFloatingWeapon(UObject* WorldContextObject, TSubclassOf<AProjectileWeaponFinal> WeaponClass, FVector SpawnLocation, FRotator SpawnRotation);
	
};
