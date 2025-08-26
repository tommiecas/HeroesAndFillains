// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PCPickupBaseItem.h"
#include "Interfaces/PickupInterface.h"
#include "Items/PrePackagedPCPickupItem.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API ATreasure : public APrePackagedPCPickupItem
{
	GENERATED_BODY()

public:
	ATreasure();
	virtual void EnableCustomDepth(bool bEnable) override;


protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 GoldValue;

public:
	FORCEINLINE int32 GetGoldValue() const { return GoldValue; }
	
};
