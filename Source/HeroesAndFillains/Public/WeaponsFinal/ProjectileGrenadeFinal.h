// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/ProjectileFinal.h"
#include "ProjectileGrenadeFinal.generated.h"

/**
 *
 */

UCLASS()

class HEROESANDFILLAINS_API AProjectileGrenadeFinal : public AProjectileFinal
{

	GENERATED_BODY()

public:
	AProjectileGrenadeFinal();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

};