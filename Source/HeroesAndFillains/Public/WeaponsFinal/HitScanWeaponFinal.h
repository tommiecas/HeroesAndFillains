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

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	

	

	
};
