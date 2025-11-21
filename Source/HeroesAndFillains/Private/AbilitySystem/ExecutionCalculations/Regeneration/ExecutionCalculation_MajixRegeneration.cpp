// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_MajixRegeneration.h"

#include "AbilitySystem/HAFAttributeSet.h"

UExecutionCalculation_MajixRegeneration::UExecutionCalculation_MajixRegeneration()
{
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetWisdomAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetCharismaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
}

void UExecutionCalculation_MajixRegeneration::Execute_Implementation(
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
	
	float Wisdom = 0.f;
	float Charisma = 0.f;

	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetWisdomCapture(), EvaluateParameters, Wisdom);
	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetCharismaCapture(), EvaluateParameters, Charisma);

	const float RegenerationScale = 0.003f;

	float Regeneration = (Wisdom + Charisma) * RegenerationScale;

	if (Regeneration > 0.f)
	{
		OutOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHAFAttributeSet::GetMajixAttribute(), EGameplayModOp::Additive, Regeneration));
	}
}
