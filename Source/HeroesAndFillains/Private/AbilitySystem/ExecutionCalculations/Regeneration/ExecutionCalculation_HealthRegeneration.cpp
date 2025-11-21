// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_HealthRegeneration.h"

#include "AbilitySystem/HAFAttributeSet.h"

UExecutionCalculation_HealthRegeneration::UExecutionCalculation_HealthRegeneration()
{
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetVigorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
}

void UExecutionCalculation_HealthRegeneration::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& Params, FGameplayEffectCustomExecutionOutput& OutOutput) const
{
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Params.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Params.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();
	
	const FGameplayTagContainer* OwnerTags =
	Params.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();

	if (OwnerTags && OwnerTags->HasTag(FGameplayTag::RequestGameplayTag("State.Combat")))
	{
		return; // Regen paused
	}
	
	float Vigor = 0.f;
	float Intelligence = 0.f;

	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetVigorCapture(), EvaluateParameters, Vigor);
	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetIntelligenceCapture(), EvaluateParameters, Intelligence);

	const float RegenerationScale = 0.003f;

	float Regeneration = (Vigor + Intelligence) * RegenerationScale;

	if (Regeneration > 0.f)
	{
		OutOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHAFAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, Regeneration));
	}
}
