// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/Ranged/ProjectileFinal.h"
#include "ProjectileRocketFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AProjectileRocketFinal : public AProjectileFinal
{
	GENERATED_BODY()

public:
	AProjectileRocketFinal();
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
protected:
	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponentFinal* RocketMovementComponent;

private:

	
};
