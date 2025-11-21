// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionCalculation_Damage.h"
#include "ExecutionCalculation_Damage_Thrope.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UExecutionCalculation_Damage_Thrope : public UExecutionCalculation_Damage
{
	GENERATED_BODY()

public:
	UExecutionCalculation_Damage_Thrope();

	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
