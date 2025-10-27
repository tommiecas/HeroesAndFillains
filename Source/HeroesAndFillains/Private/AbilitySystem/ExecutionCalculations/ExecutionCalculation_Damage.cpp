// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/ExecutionCalculation_Damage.h"

#include "AbilitySystemComponent.h"
#include "HAFAbilityTypes.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"

struct HAFDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Fireproof);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Shockproof);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChaosIncorruptible);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Invulnerability);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HeartOfDarkness);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ThermalRadiation);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Immunity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Unstoppable);
	
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	HAFDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Fireproof, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Shockproof, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ChaosIncorruptible, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Invulnerability, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, HeartOfDarkness, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ThermalRadiation, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Immunity, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Unstoppable, Target, false);

		const FHAFGameplayTags& Tags = FHAFGameplayTags::Get();
		
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireproofDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, ShockproofDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_ChaosMajix, ChaosIncorruptibleDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_MeleeAttacks, InvulnerabilityDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_RuleOfOrder, HeartOfDarknessDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Ice, ThermalRadiationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Toxicity, ImmunityDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Stun, UnstoppableDef);
	}
};

static const HAFDamageStatics& DamageStatics()
{
	static HAFDamageStatics DStatics;
	return DStatics;
}
UExecutionCalculation_Damage::UExecutionCalculation_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireproofDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShockproofDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChaosIncorruptibleDef);
	RelevantAttributesToCapture.Add(DamageStatics().InvulnerabilityDef);
	RelevantAttributesToCapture.Add(DamageStatics().HeartOfDarknessDef);
	RelevantAttributesToCapture.Add(DamageStatics().ThermalRadiationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ImmunityDef);
	RelevantAttributesToCapture.Add(DamageStatics().UnstoppableDef);

}

void UExecutionCalculation_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// PART ONE: Get Damage Set by Caller Magnitude
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FHAFGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTypeTag = Pair.Value;

		checkf(HAFDamageStatics().TagsToCaptureDefs.Contains(ResistanceTypeTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTypeTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = HAFDamageStatics().TagsToCaptureDefs[ResistanceTypeTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;
		
		// If the source is resistant to the damage type, add the damage type's value to the damage.
		Damage += DamageTypeValue;
	}

	// PART TWO: Capture BLOCKCHANCE on Target and determine if there was a successful BLOCK
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	// If it was blocked, cut the damage by 50%.
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UHAFAbilitySystemBlueprintLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	Damage = bBlocked ? Damage / 2.f : Damage;

	//PART THREE: Capture Armor on Target, but Armor Penetration on Source.
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UHAFAbilitySystemBlueprintLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetrationCoefficient"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	//ArmorPenetration ignores a portion of the target's armor... 
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	//...while Armor removes part of the damage from the source.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmorCoefficient"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	//Armor is a percentage of the damage taken.
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UHAFAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

	//PART FOUR: Critical Hit Resistance reduces Critical Hit Chance, while Double Damage Plus Bonus if a Critical Hit is made
	// Critical hit calculation
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	const FRealCurve* CriticalHitResistanceCurve =
		CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistanceCoefficient"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UHAFAbilitySystemBlueprintLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	if (bCriticalHit)
	{
		Damage = 2.f * Damage + SourceCriticalHitDamage;
	}

	// ✅ NOW apply final damage to IncomingDamage
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UHAFAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage));
	/*float Damage = Spec.EffectSpec.GetMagnitude() * (1.f - (Armor / 100.f));
	OutExecutionOutput.AddDamage(Damage);
	//
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, )*/
}
