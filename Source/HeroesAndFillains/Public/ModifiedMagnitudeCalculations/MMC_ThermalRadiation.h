
#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ThermalRadiation.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMMC_ThermalRadiation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ThermalRadiation();

	UFUNCTION(BlueprintCallable)
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition ResilienceDef;
	FGameplayEffectAttributeCaptureDefinition WisdomDef;
	
	
};
