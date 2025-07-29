// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MajixPickup.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AMajixPickup : public APickup
{
	GENERATED_BODY()

public:
	AMajixPickup();

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
	float MajixSummonsAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float MajixSummonsTime = 5.f;


	
};
