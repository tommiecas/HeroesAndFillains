// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/ProjectileFinal.h"
#include "ProjectileRocketFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AProjectileRocketFinal : public AProjectileFinal
{
	GENERATED_BODY()

protected:
	AProjectileRocketFinal();
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
	
};
