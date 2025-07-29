// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "StaminaPickup.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AStaminaPickup : public APickup
{
	GENERATED_BODY()

public:
	AStaminaPickup();

protected:
	virtual void OnSphereOverlap(
		class UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
	float StaminaRechargeAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float StaminaRechargeTime = 5.f;


	
};
