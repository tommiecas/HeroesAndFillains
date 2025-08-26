#include "ModifiedMagnitudeCalculations/MMC_Flexibility.h"
#include "AbilitySystem/HAFAttributeSet.h"

UMMC_Flexibility::UMMC_Flexibility()
{
	DexterityDef.AttributeToCapture = UHAFAttributeSet::GetDexterityAttribute();
	DexterityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DexterityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(DexterityDef);
}

float UMMC_Flexibility::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Dexterity = 0.f;

	GetCapturedAttributeMagnitude(DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);
	
	return (((Dexterity - 5.f) * 0.20f) + 1.f); 
}
