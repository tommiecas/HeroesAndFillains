#include "ModifiedMagnitudeCalculations/MMC_MaxShield.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxShield::UMMC_MaxShield()
{
	ResilienceDef.AttributeToCapture = UHAFAttributeSet::GetResilienceAttribute();
	ResilienceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ResilienceDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(ResilienceDef);

	ArmorDef.AttributeToCapture = UHAFAttributeSet::GetArmorAttribute();
	ArmorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ArmorDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(ArmorDef);

	StrengthDef.AttributeToCapture = UHAFAttributeSet::GetStrengthAttribute();
	StrengthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrengthDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_MaxShield::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	float Resilience = 0.f;
	float Armor = 0.f;
	float Strength = 0.f;

	GetCapturedAttributeMagnitude(ResilienceDef, Spec, Params, Resilience);
	GetCapturedAttributeMagnitude(ArmorDef, Spec, Params, Armor);
	GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength);

	Resilience = FMath::Max(Resilience, 0.f);
	Armor      = FMath::Max(Armor, 0.f);
	Strength   = FMath::Max(Strength, 0.f);

	// (No level dependency here, so no CombatInterface required)
	return (25.f + (1.8f * Resilience)) +
		   (25.f + (1.8f * Armor)) +
		   (25.f + (1.8f * Strength));
}
