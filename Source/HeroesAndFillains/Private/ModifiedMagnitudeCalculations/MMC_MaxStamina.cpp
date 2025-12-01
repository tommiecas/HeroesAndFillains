#include "ModifiedMagnitudeCalculations/MMC_MaxStamina.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxStamina::UMMC_MaxStamina()
{
	VigorDef.AttributeToCapture = UHAFAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(VigorDef);

	DexterityDef.AttributeToCapture = UHAFAttributeSet::GetDexterityAttribute();
	DexterityDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DexterityDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(DexterityDef);

	StrengthDef.AttributeToCapture = UHAFAttributeSet::GetStrengthAttribute();
	StrengthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrengthDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	float Vigor = 0.f;
	float Dexterity = 0.f;
	float Strength = 0.f;

	GetCapturedAttributeMagnitude(VigorDef, Spec, Params, Vigor);
	GetCapturedAttributeMagnitude(DexterityDef, Spec, Params, Dexterity);
	GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength);

	Vigor     = FMath::Max(Vigor, 0.f);
	Dexterity = FMath::Max(Dexterity, 0.f);
	Strength  = FMath::Max(Strength, 0.f);
	
	// (Stamina might later scale with PlayerLevel, so ready for safe check)
	int32 CharacterLevel = 1; // Default level if context missing
	UObject* SourceObj = Spec.GetContext().GetSourceObject();

	if (SourceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_MaxStamina: SourceObject is NULL (likely during respawn)."));
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
			UE_LOG(LogTemp, Warning, TEXT("UMMC_MaxStamina: SourceObject %s does not implement CombatInterface."),
				*SourceObj->GetName());
		}
		
		// (No level used here either, safe from null SourceObject)
		return (30.f + (2.f * Vigor)) +
			(30.f + (2.f * Dexterity)) +
			(30.f + (2.f * Strength));
	}
	return 0.f;
}
