// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_ChaosIncorruptability.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Interfaces/CombatInterface.h"

UMMC_ChaosIncorruptability::UMMC_ChaosIncorruptability()
{
	ResilienceDef.AttributeToCapture = UHAFAttributeSet::GetResilienceAttribute();
	ResilienceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ResilienceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(ResilienceDef);

	PurityDef.AttributeToCapture = UHAFAttributeSet::GetPurityAttribute();
	PurityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	PurityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(PurityDef);
}

float UMMC_ChaosIncorruptability::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Resilience = 0.f;
	GetCapturedAttributeMagnitude(ResilienceDef, Spec, EvaluationParameters, Resilience);
	Resilience = FMath::Max<float>(Resilience, 0.f);

	float Purity = 0.f;
	GetCapturedAttributeMagnitude(PurityDef, Spec, EvaluationParameters, Purity);
	Purity = FMath::Max<float>(Purity, 0.f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return (1.5f + (.25f * Resilience)) + (1.5f + (.25 * Purity));
}
