// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API ATreasure : public AItem
{
	GENERATED_BODY()

public:
	ATreasure();

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	UStaticMeshComponent* TreasureMesh;

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold; 
	
};
