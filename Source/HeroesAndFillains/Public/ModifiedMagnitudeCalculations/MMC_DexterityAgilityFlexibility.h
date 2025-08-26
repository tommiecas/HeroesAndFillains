// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_DexterityAgilityFlexibility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_DexterityAgilityFlexibility : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_DexterityAgilityFlexibility();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition AgilityDef;
	FGameplayEffectAttributeCaptureDefinition FlexibilityDef;
	
	
};
