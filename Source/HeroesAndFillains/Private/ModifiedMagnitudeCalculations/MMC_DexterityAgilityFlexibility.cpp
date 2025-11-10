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
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	float Dexterity = 0.f;
	float Agility = 0.f;
	float Flexibility = 0.f;
	GetCapturedAttributeMagnitude(DexterityDef, Spec, Params, Dexterity);
	GetCapturedAttributeMagnitude(AgilityDef, Spec, Params, Agility);
	GetCapturedAttributeMagnitude(FlexibilityDef, Spec, Params, Flexibility);

	Dexterity   = FMath::Max(Dexterity, 0.f);
	Agility     = FMath::Max(Agility, 0.f);
	Flexibility = FMath::Max(Flexibility, 0.f);

	// --- Safe Character and Capsule access ---
	const ABaseCharacter* Character = Cast<ABaseCharacter>(Spec.GetContext().GetSourceObject());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_DexterityAgilityFlexibility: SourceObject is NULL or not a BaseCharacter."));
		return 0.f;
	}

	const UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	if (!Capsule)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMMC_DexterityAgilityFlexibility: CapsuleComponent missing on %s"), *Character->GetName());
		return 0.f;
	}

	// --- Original math preserved ---
	const float AgilityPercent     = Agility / 100.f;
	const float FlexibilityPercent = Flexibility / 100.f;

	const float OldHalf   = Capsule->GetUnscaledCapsuleHalfHeight();
	const float OldRadius = Capsule->GetUnscaledCapsuleRadius();

	const float NewHalf   = OldHalf   * (1.f - AgilityPercent);
	const float NewRadius = OldRadius * (1.f - FlexibilityPercent);

	// (Optional debug)
	// UE_LOG(LogTemp, Warning, TEXT("DexAgiFlex: Half=%.2f -> %.2f | Radius=%.2f -> %.2f"), OldHalf, NewHalf, OldRadius, NewRadius);

	return NewHalf - NewRadius;
}
