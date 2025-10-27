// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_DexterityAgilityFlexibility.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/CapsuleInterface.h"

UMMC_DexterityAgilityFlexibility::UMMC_DexterityAgilityFlexibility()
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
}

float UMMC_DexterityAgilityFlexibility::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Dexterity = 0.f;
	float Agility = 0.f;
	float Flexibility = 0.f;
	GetCapturedAttributeMagnitude(DexterityDef, Spec, EvaluationParameters, Dexterity);
	GetCapturedAttributeMagnitude(AgilityDef, Spec, EvaluationParameters, Agility);
	GetCapturedAttributeMagnitude(FlexibilityDef, Spec, EvaluationParameters, Flexibility);
	
	const ABaseCharacter* Character = Cast<ABaseCharacter>(Spec.GetContext().GetSourceObject());
	const float AgilityPercent      = Agility / 100.f;
	const float FlexibilityPercent  = Flexibility / 100.f;

	const float OldHalf   = Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldRadius = Character->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	const float NewHalf   = OldHalf   * (1.f - AgilityPercent);
	const float NewRadius = OldRadius * (1.f - FlexibilityPercent);
	
	// UE_LOG(LogTemp, Warning, TEXT("MMC (debug): half=%.2f radius=%.2f"), NewHalf, NewRadius);
	return NewHalf - NewRadius;	
}

