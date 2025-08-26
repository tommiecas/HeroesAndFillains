// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Intuition.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_Intuition : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_Intuition();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition WisdomDef;
	FGameplayEffectAttributeCaptureDefinition VisionDef;
	
	
};
