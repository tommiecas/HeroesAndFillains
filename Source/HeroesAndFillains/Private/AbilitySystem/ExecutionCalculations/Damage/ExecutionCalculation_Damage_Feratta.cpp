// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/ExecutionCalculation_Damage_Feratta.h"

#include "AbilitySystemComponent.h"
#include "HAFAbilityTypes.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystem/ExecutionCalculations/HAFDamageStatics.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"

UExecutionCalculation_Damage_Feratta::UExecutionCalculation_Damage_Feratta()
{
	
	RelevantAttributesToCapture.Add(HAFDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(HAFDamageStatics().FireproofDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().ShockproofDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().ChaosIncorruptibleDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().InvulnerabilityDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().HeartOfDarknessDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().ThermalRadiationDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().ImmunityDef);
	RelevantAttributesToCapture.Add(HAFDamageStatics().UnstoppableDef);

}

void UExecutionCalculation_Damage_Feratta::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourceCharacterLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		if (ABaseCharacter* SourceAvBaseChar = Cast<ABaseCharacter>(SourceAvatar))
		{
			SourceCharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceAvatar, SourceAvBaseChar);	
		}
	}
	int32 TargetCharacterLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		if (ABaseCharacter* TargetAvBaseChar = Cast<ABaseCharacter>(TargetAvatar))
		{
			TargetCharacterLevel = ICombatInterface::Execute_GetCharacterLevel(TargetAvatar, TargetAvBaseChar);	
		}
	}

	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// PART ONE: Get Damage Set by Caller Magnitude
	float Damage = 15.f;
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
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	// If it was blocked, cut the damage by 50%.
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UHAFAbilitySystemBlueprintLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	Damage = bBlocked ? Damage / 2.f : Damage;

	//PART THREE: Capture Armor on Target, but Armor Penetration on Source.
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UHAFAbilitySystemBlueprintLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetrationCoefficient"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCharacterLevel);
	//ArmorPenetration ignores a portion of the target's armor... 
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	//...while Armor removes part of the damage from the source.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmorCoefficient"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCharacterLevel);
	//Armor is a percentage of the damage taken.
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UHAFAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

	//PART FOUR: Critical Hit Resistance reduces Critical Hit Chance, while Double Damage Plus Bonus if a Critical Hit is made
	// Critical hit calculation
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	const FRealCurve* CriticalHitResistanceCurve =
		CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistanceCoefficient"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCharacterLevel);

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

	UE_LOG(LogTemp, Warning, TEXT("💥 DAMAGE CALCULATED: %.1f"), Damage);
    
	
}
