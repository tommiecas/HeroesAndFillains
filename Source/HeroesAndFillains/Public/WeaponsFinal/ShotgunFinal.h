// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/HitScanWeaponFinal.h"
#include "ShotgunFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AShotgunFinal : public AHitScanWeaponFinal
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

protected:

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	uint32 Buckshot = 10;
};