// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_StaminaRegeneration.h"

#include "AbilitySystem/HAFAttributeSet.h"

UExecutionCalculation_StaminaRegeneration::UExecutionCalculation_StaminaRegeneration()
{
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
	RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UHAFAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
}

void UExecutionCalculation_StaminaRegeneration::Execute_Implementation(
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
	
	float Strength = 0.f;
	float Dexterity = 0.f;

	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetStrengthCapture(), EvaluateParameters, Strength);
	Params.AttemptCalculateCapturedAttributeMagnitude(UHAFAttributeSet::GetDexterityCapture(), EvaluateParameters, Dexterity);

	const float RegenerationScale = 0.003f;

	float Regeneration = (Strength + Dexterity) * RegenerationScale;

	if (Regeneration > 0.f)
	{
		OutOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHAFAttributeSet::GetStaminaAttribute(), EGameplayModOp::Additive, Regeneration));
	}
}
