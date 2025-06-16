// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Interfaces/HitInterface.h"
#include "Gnarled.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AGnarled : public AEnemyBase
{
	GENERATED_BODY()

public:
	AGnarled();
	virtual void Tick(float DeltaTime) override;
	void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

private:
	
	
};
