// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Shockproof.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_Shockproof : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_Shockproof();

	UFUNCTION(BlueprintCallable)
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition ResilienceDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	
	
};
