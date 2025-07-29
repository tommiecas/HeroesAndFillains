// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AEnemyBase* OriginEnemy;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 SoulValue;

public:
	int32 GetSoulValue() const;
	void SetSoulValue(int32 Value);
	FORCEINLINE AEnemyBase* GetOriginEnemy() const { return OriginEnemy; }
	FORCEINLINE void SetOriginEnemy(AEnemyBase* Enemy) { OriginEnemy = Enemy; }

};
