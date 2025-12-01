#include "ModifiedMagnitudeCalculations/MMC_MaxHealth.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UHAFAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;

	// --- Capture Vigor safely ---
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParams, Vigor);
	Vigor = FMath::Max(Vigor, 0.f);

	// --- Safe CombatInterface access ---
	int32 CharacterLevel = 1; // Default level if context missing
	UObject* SourceObj = Spec.GetContext().GetSourceObject();

	if (SourceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_MaxHealth: SourceObject is NULL (likely during respawn)."));
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
			UE_LOG(LogTemp, Warning, TEXT("UMMC_MaxHealth: SourceObject %s does not implement CombatInterface."),
				*SourceObj->GetName());
		}

		// --- Your original formula (unchanged functionality) ---
		const float MaxHealth = 80.f + (2.5f * Vigor) + (10.f * CharacterLevel);

		return MaxHealth;
	}
	return 0.f;
}
