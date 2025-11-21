// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecutionCalculation_ShieldRegeneration.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UExecutionCalculation_ShieldRegeneration : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecutionCalculation_ShieldRegeneration();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Params, FGameplayEffectCustomExecutionOutput& OutOutput) const override;
};
