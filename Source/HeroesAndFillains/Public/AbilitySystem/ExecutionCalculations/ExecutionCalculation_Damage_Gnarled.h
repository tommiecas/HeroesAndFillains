// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionCalculation_Damage.h"
#include "ExecutionCalculation_Damage_Gnarled.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UExecutionCalculation_Damage_Gnarled : public UExecutionCalculation_Damage
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
