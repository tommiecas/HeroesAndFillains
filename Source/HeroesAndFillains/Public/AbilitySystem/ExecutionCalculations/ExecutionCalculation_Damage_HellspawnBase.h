// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionCalculation_Damage.h"
#include "ExecutionCalculation_Damage_HellspawnBase.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UExecutionCalculation_Damage_HellspawnBase : public UExecutionCalculation_Damage
{
	GENERATED_BODY()

public:
	UExecutionCalculation_Damage_HellspawnBase();

	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
