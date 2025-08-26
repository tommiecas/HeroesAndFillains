// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_Vision.h"

#include "AbilitySystem/HAFAttributeSet.h"

UMMC_Vision::UMMC_Vision()
{
	MarksmanshipDef.AttributeToCapture = UHAFAttributeSet::GetMarksmanshipAttribute();
	MarksmanshipDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MarksmanshipDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(MarksmanshipDef);

	IntelligenceDef.AttributeToCapture = UHAFAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelligenceDef);

	VigorDef.AttributeToCapture = UHAFAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_Vision::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Marksmanship = 0.f;
	float Intelligence = 0.f;
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(MarksmanshipDef, Spec, EvaluationParameters, Marksmanship);
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluationParameters, Intelligence);
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	
	Marksmanship = FMath::Max<float>(Marksmanship, 0.f);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);
	Vigor = FMath::Max<float>(Vigor, 0.f);
	
	return ((Marksmanship * .10f) + 2.f) + ((Intelligence * .10f) + 2.f) + ((Vigor * .10f) + 2.f);
}
