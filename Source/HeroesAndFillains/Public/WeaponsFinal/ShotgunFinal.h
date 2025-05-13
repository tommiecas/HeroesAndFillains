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
	void ShotgunFinalTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);
	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 BuckshotCount = 10;
};