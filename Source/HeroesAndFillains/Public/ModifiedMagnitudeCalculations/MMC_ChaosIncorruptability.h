// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ChaosIncorruptability.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_ChaosIncorruptability : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ChaosIncorruptability();

	UFUNCTION(BlueprintCallable)
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition ResilienceDef;
	FGameplayEffectAttributeCaptureDefinition PurityDef;
	
};
