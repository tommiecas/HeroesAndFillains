// Fill out your copyright notice in the Description page of Project Settings.

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
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Resilience = 0.f;
	float Armor = 0.f;
	float Strength = 0.f;

	GetCapturedAttributeMagnitude(ResilienceDef, Spec, EvaluationParameters, Resilience);
	Resilience = FMath::Max<float>(Resilience, 0.f);

	GetCapturedAttributeMagnitude(ArmorDef, Spec, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(Armor, 0.f);

	GetCapturedAttributeMagnitude(StrengthDef, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);
	
	return (25.f + (1.8f * Resilience)) + (25.f + (1.8 * Armor)) + (25.f + (1.8 * Strength));
}
