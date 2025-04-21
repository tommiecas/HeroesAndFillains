// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "ProjectileWeaponFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AProjectileWeaponFinal : public AWeaponFinal
{
	GENERATED_BODY()

public: 
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileFinal> ProjectileFinalClass;
	
};
