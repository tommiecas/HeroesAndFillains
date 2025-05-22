// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsFinal/Ranged/ProjectileFinal.h"
#include "HitScanWeaponAmmo.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AHitScanWeaponAmmo : public AProjectileFinal
{
	GENERATED_BODY()
public:
	AHitScanWeaponAmmo();
	virtual void Destroyed() override;

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	// void DestroyTimerFinished();



private:


	UPROPERTY()
	class AProjectileFinal* ProjectileFinal;




	FTimerHandle DestroyTimer;

	

};

