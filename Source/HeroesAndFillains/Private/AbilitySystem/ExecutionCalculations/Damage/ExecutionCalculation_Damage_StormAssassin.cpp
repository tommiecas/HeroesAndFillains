// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/ExecutionCalculation_Damage_StormAssassin.h"

#include "AbilitySystemComponent.h"
#include "HAFAbilityTypes.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystem/ExecutionCalculations/HAFDamageStatics.h"
#include "Characters/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"

UExecutionCalculation_Damage_StormAssassin::UExecutionCalculation_Damage_StormAssassin()
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

void UExecutionCalculation_Damage_StormAssassin::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// ✅ Validate ASC components
	if (!SourceASC || !TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: Invalid ASC components!"));
		return;
	}

	AActor* SourceAvatar = SourceASC->GetAvatarActor();
	AActor* TargetAvatar = TargetASC->GetAvatarActor();

	// ✅ Validate avatars
	if (!SourceAvatar || !TargetAvatar)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: Invalid avatar actors!"));
		return;
	}

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	// ✅ Validate combat interfaces
	if (!SourceCombatInterface || !TargetCombatInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: Actors don't implement CombatInterface!"));
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// PART ONE: Get Damage Set by Caller Magnitude
	float Damage = 10.f;
	
	// ✅ FIX: Get the static instance properly using DamageStatics() each time
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FHAFGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTypeTag = Pair.Value;

		if (!HAFDamageStatics().TagsToCaptureDefs.Contains(ResistanceTypeTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("TagsToCaptureDefs doesn't contain Tag: [%s]"), *ResistanceTypeTag.ToString());
			continue;
		}

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = HAFDamageStatics().TagsToCaptureDefs[ResistanceTypeTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false, 0.f);

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
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UHAFAbilitySystemBlueprintLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	Damage = bBlocked ? Damage / 2.f : Damage;

	// PART THREE: Capture Armor on Target, but Armor Penetration on Source.
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// ✅ Validate CharacterClassInfo
	const UCharacterClassInfo* CharacterClassInfo = UHAFAbilitySystemBlueprintLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: CharacterClassInfo is null!"));
		return;
	}

	// ✅ Validate DamageCalculationCoefficients
	if (!CharacterClassInfo->DamageCalculationCoefficients)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: DamageCalculationCoefficients is null!"));
		return;
	}

	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetrationCoefficient"), FString());
	if (!ArmorPenetrationCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: ArmorPenetrationCurve not found!"));
		return;
	}

	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmorCoefficient"), FString());
	if (!EffectiveArmorCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: EffectiveArmorCurve not found!"));
		return;
	}

	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// PART FOUR: Critical Hit Resistance reduces Critical Hit Chance, Double Damage Plus Bonus if Critical Hit
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HAFDamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistanceCoefficient"), FString());
	if (!CriticalHitResistanceCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecutionCalculation_Damage_StormAssassin: CriticalHitResistanceCurve not found!"));
		return;
	}

	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UHAFAbilitySystemBlueprintLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	if (bCriticalHit)
	{
		Damage = 2.f * Damage + SourceCriticalHitDamage;
	}

	// ✅ Final damage output
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UHAFAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage));

	UE_LOG(LogTemp, Warning, TEXT("💥 DAMAGE CALCULATED: %.1f (Critical: %s, Blocked: %s)"), 
		Damage, bCriticalHit ? TEXT("YES") : TEXT("NO"), bBlocked ? TEXT("YES") : TEXT("NO"));
}
