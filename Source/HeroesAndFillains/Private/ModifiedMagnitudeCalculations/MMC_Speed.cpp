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
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(DexterityDef, Spec, Params, Dexterity);
	Dexterity = FMath::Max(Dexterity, 0.f);

	float Agility = 0.f;
	GetCapturedAttributeMagnitude(AgilityDef, Spec, Params, Agility);
	Agility = FMath::Max(Agility, 0.f);

	float Flexibility = 0.f;
	GetCapturedAttributeMagnitude(FlexibilityDef, Spec, Params, Flexibility);
	Flexibility = FMath::Max(Flexibility, 0.f);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength);
	Strength = FMath::Max(Strength, 0.f);

	// ------------------------------------------------------------
	// SAFE COMBAT INTERFACE HANDLING
	// ------------------------------------------------------------
	int32 PlayerLevel = 1; // Safe default
	UObject* SourceObj = Spec.GetContext().GetSourceObject();

	if (SourceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_Speed: SourceObject is NULL (likely during respawn)."));
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(SourceObj))
	{
		PlayerLevel = CombatInterface->GetPlayerLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_Speed: SourceObject %s does not implement CombatInterface."),
			*SourceObj->GetName());
	}

	// Prevent divide-by-zero if Agility + Flexibility == 0
	const float Denominator = FMath::Max(Agility + Flexibility, KINDA_SMALL_NUMBER);

	float BaseSpeed = 0.f;
	if (Dexterity >= Strength)
	{
		BaseSpeed = (((Dexterity - Strength) / Denominator) + 2.f) / 5.f;
	}
	else
	{
		const float PositiveDiff = FMath::Abs(Dexterity - Strength);
		BaseSpeed = (PositiveDiff / Denominator) + 2.f;
	}

	return BaseSpeed;
}


