#include "ModifiedMagnitudeCalculations/MMC_MaxMajix.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxMajix::UMMC_MaxMajix()
{
	IntelligenceDef.AttributeToCapture = UHAFAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(IntelligenceDef);

	WisdomDef.AttributeToCapture = UHAFAttributeSet::GetWisdomAttribute();
	WisdomDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	WisdomDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(WisdomDef);
}

float UMMC_MaxMajix::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	float Intelligence = 0.f;
	float Wisdom = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, Params, Intelligence);
	GetCapturedAttributeMagnitude(WisdomDef, Spec, Params, Wisdom);

	Intelligence = FMath::Max(Intelligence, 0.f);
	Wisdom       = FMath::Max(Wisdom, 0.f);

	// (Majix might later scale with PlayerLevel, so ready for safe check)
	int32 PlayerLevel = 1;
	UObject* SourceObj = Spec.GetContext().GetSourceObject();
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(SourceObj))
	{
		PlayerLevel = CombatInterface->GetPlayerLevel();
	}

	return ((40.f + (2.5f * Intelligence)) +
			(40.f + (2.5f * Wisdom))) + (5.f * PlayerLevel);
}
