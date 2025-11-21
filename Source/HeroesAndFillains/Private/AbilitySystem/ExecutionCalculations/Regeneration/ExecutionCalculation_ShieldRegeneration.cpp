// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_ShieldRegeneration.h"

#include "AbilitySystem/HAFAttributeSet.h"

UExecutionCalculation_ShieldRegeneration::UExecutionCalculation_ShieldRegeneration()
{
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetResilienceAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetMarksmanshipAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
}

void UExecutionCalculation_ShieldRegeneration::Execute_Implementation(
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
	
	float Resilience = 0.f;
	float Marksmanship = 0.f;

	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetResilienceCapture(), EvaluateParameters, Resilience);
	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetMarksmanshipCapture(), EvaluateParameters, Marksmanship);

	const float RegenerationScale = 0.003f;

	float Regeneration = (Resilience + Marksmanship) * RegenerationScale;

	if (Regeneration > 0.f)
	{
		OutOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHAFAttributeSet::GetShieldAttribute(), EGameplayModOp::Additive, Regeneration));
	}
}
