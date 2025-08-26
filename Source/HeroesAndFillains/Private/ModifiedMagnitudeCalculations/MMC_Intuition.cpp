// Fill out your copyright notice in the Description page of Project Settings.

#include "ModifiedMagnitudeCalculations/MMC_Intuition.h"

#include "AbilitySystem/HAFAttributeSet.h"

UMMC_Intuition::UMMC_Intuition()
{
	DexterityDef.AttributeToCapture = UHAFAttributeSet::GetDexterityAttribute();
	DexterityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DexterityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(DexterityDef);

	WisdomDef.AttributeToCapture = UHAFAttributeSet::GetWisdomAttribute();
	WisdomDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	WisdomDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(WisdomDef);

	VisionDef.AttributeToCapture = UHAFAttributeSet::GetVisionAttribute();
	VisionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VisionDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VisionDef);
}

float UMMC_Intuition::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Dexterity = 0.f;
	float Wisdom = 0.f;
	float Vision = 0.f;
	GetCapturedAttributeMagnitude(DexterityDef, Spec, EvaluationParameters, Dexterity);
	GetCapturedAttributeMagnitude(WisdomDef, Spec, EvaluationParameters, Wisdom);
	GetCapturedAttributeMagnitude(VisionDef, Spec, EvaluationParameters, Vision);
	
	Dexterity = FMath::Max<float>(Dexterity, 0.f);
	Wisdom = FMath::Max<float>(Wisdom, 0.f);
	Vision = FMath::Max<float>(Vision, 0.f);
	
	return ((Dexterity * .25f) + 2.f) + ((Wisdom * .25f) + 3.f) + ((Vision * .10f) + 2.f);
}
