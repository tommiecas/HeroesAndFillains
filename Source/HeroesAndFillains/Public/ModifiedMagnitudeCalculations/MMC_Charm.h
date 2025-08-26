// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Charm.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_Charm : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_Charm();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition CharismaDef;
};
