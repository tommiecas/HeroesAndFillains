// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/ProjectileFinal.h"
#include "ProjectileBulletFinal.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AProjectileBulletFinal : public AProjectileFinal
{
	GENERATED_BODY()


protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	
};
