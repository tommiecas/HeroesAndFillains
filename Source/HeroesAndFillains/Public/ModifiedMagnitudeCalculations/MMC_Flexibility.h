// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Flexibility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_Flexibility : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_Flexibility();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition DexterityDef;

	
	
};
