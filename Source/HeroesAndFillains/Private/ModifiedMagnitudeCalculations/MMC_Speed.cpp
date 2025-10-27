// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_Speed.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Interfaces/CombatInterface.h"

UMMC_Speed::UMMC_Speed()
{
	DexterityDef.AttributeToCapture = UHAFAttributeSet::GetDexterityAttribute();
	DexterityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DexterityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(DexterityDef);

	AgilityDef.AttributeToCapture = UHAFAttributeSet::GetAgilityAttribute();
	AgilityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	AgilityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AgilityDef);

	FlexibilityDef.AttributeToCapture = UHAFAttributeSet::GetFlexibilityAttribute();
	FlexibilityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	FlexibilityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(FlexibilityDef);

	StrengthDef.AttributeToCapture = UHAFAttributeSet::GetStrengthAttribute();
	StrengthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrengthDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StrengthDef);

	
}

float UMMC_Speed::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	float Agility = 0.f;
	GetCapturedAttributeMagnitude(AgilityDef, Spec, EvaluationParameters, Agility);
	Agility = FMath::Max<float>(Agility, 0.f);

	float Flexibility = 0.f;
	GetCapturedAttributeMagnitude(FlexibilityDef, Spec, EvaluationParameters, Flexibility);
	Flexibility = FMath::Max<float>(Flexibility, 0.f);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	if (Dexterity > Strength)
	{
		return (((Dexterity - Strength) / (Agility + Flexibility)) + 2.f) / 5.f;
	}
	else
	{
		float DexStrenDiff = Dexterity - Strength; // This will be negative
		float PositiveDexStrenDiff = DexStrenDiff - DexStrenDiff - DexStrenDiff; // Converts negative to positive
		return (PositiveDexStrenDiff / (Agility + Flexibility)) + 2.f;
	}
}

