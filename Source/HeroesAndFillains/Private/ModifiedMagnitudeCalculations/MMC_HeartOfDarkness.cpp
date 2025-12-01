// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifiedMagnitudeCalculations/MMC_HeartOfDarkness.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/CombatInterface.h"

UMMC_HeartOfDarkness::UMMC_HeartOfDarkness()
{
	ResilienceDef.AttributeToCapture = UHAFAttributeSet::GetResilienceAttribute();
	ResilienceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ResilienceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(ResilienceDef);

	CorruptibilityDef.AttributeToCapture = UHAFAttributeSet::GetCorruptibilityAttribute();
	CorruptibilityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	CorruptibilityDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(CorruptibilityDef);
}

float UMMC_HeartOfDarkness::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
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

	float Corruptibility = 0.f;
	GetCapturedAttributeMagnitude(CorruptibilityDef, Spec, EvaluationParameters, Corruptibility);
	Corruptibility = FMath::Max<float>(Corruptibility, 0.f);
	
	int32 CharacterLevel = 1; // Default level if context missing
	UObject* SourceObj = Spec.GetContext().GetSourceObject();

	if (SourceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_HeartOfDarkness: SourceObject is NULL (likely during respawn)."));
	}
	else 
	{
		if (SourceObj->Implements<UCombatInterface>())
		{
			if (ABaseCharacter* SourceChar = Cast<ABaseCharacter>(SourceObj))
			{
				CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObj, SourceChar);	
			}
		}
		else
		{	
			UE_LOG(LogTemp, Warning, TEXT("UMMC_HeartOfDarkness: SourceObject %s does not implement CombatInterface."),
				*SourceObj->GetName());
		}

		return (1.5f + (.25f * Resilience)) + (1.5f + (.25 * Corruptibility));
	}
	return 0.f;
}
	
