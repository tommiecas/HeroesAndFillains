// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "HitScanWeaponFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AHitScanWeaponFinal : public AWeaponFinal
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	
};
