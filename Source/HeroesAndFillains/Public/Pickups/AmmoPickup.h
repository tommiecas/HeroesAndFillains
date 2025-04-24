// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Weapons/WeaponTypes.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public: 
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponFinalType WeaponFinalType;

private:

	
};
