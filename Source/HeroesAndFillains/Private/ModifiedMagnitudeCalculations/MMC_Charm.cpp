// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_Charm.h"

#include "AbilitySystem/HAFAttributeSet.h"

UMMC_Charm::UMMC_Charm()
{
	CharismaDef.AttributeToCapture = UHAFAttributeSet::GetCharismaAttribute();
	CharismaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	CharismaDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(CharismaDef);
}

float UMMC_Charm::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Charisma = 0.f;

	GetCapturedAttributeMagnitude(CharismaDef, Spec, EvaluationParameters, Charisma);
	
	Charisma = FMath::Max<float>(Charisma, 0.f);
	
	return ((Charisma * .15f) + 1.f);
}
