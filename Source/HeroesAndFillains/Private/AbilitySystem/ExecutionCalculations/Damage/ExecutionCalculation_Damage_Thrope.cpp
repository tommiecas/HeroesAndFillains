// Copyright Druid Mechanics


#include "AbilitySystem/ExecutionCalculations/Damage/ExecutionCalculation_Damage_Thrope.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayEffectTypes.h"
#include "HAFGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "HAFAbilityTypes.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/ExecutionCalculations/Damage/ExecutionCalculation_DamageStatics.h"

void UExecutionCalculation_Damage_Thrope::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FHAFGameplayTags& Tags = FHAFGameplayTags::Get();

	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Agility, DamageStatics().AgilityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Flexibility, DamageStatics().FlexibilityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Purity, DamageStatics().PurityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Corruptibility, DamageStatics().CorruptibilityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Intuition, DamageStatics().IntuitionDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Vision, DamageStatics().VisionDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Charm, DamageStatics().CharmDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Speed, DamageStatics().SpeedDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_HealthRegeneration, DamageStatics().HealthRegenerationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ShieldRegeneration, DamageStatics().ShieldRegenerationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_StaminaRegeneration, DamageStatics().StaminaRegenerationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MajixRegeneration, DamageStatics().MajixRegenerationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MaxHealth, DamageStatics().MaxHealthDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MaxShield, DamageStatics().MaxShieldDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MaxStamina, DamageStatics().MaxStaminaDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MaxMajix, DamageStatics().MaxMajixDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fireproof, DamageStatics().FireproofDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Shockproof, DamageStatics().ShockproofDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_ChaosIncorruptibility, DamageStatics().ChaosIncorruptibleDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Invulnerability, DamageStatics().InvulnerabilityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_HeartOfDarkness, DamageStatics().HeartOfDarknessDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_ThermalRadiation, DamageStatics().ThermalRadiationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Immunity, DamageStatics().ImmunityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Unstoppable, DamageStatics().UnstoppableDef);
	
	if (ExecutionParams.GetSourceAbilitySystemComponent() && ExecutionParams.GetTargetAbilitySystemComponent())
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
		FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

		const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
		const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
		FAggregatorEvaluateParameters EvaluationParameters;
		EvaluationParameters.SourceTags = SourceTags;
		EvaluationParameters.TargetTags = TargetTags;

		//Debuff
		DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

		// PART ONE: Get Damage Set by Caller Magnitude

		float Damage = 15.f;
		for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Tags.DamageTypesToResistances)
		{
			const FGameplayTag DamageTypeTag = Pair.Key;
			const FGameplayTag ResistanceTypeTag = Pair.Value;

			checkf(TagsToCaptureDefs.Contains(ResistanceTypeTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTypeTag.ToString());
			const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTypeTag];

			float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
			if (DamageTypeValue <= 0.f)
			{
				continue;
			}
			float Resistance = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
			Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

			DamageTypeValue *= ( 100.f - Resistance ) / 100.f;

			if (UHAFAbilitySystemBlueprintLibrary::IsRadialDamage(EffectContextHandle))
			{
				// 1. Override Take Damage in BaseCharacter *DONE
				// 2. Create OnDamageDelegate, broadcast Damage Received in TakeDamage. *DONE
				// 3. Bind to OnDamageDelegate on the Victim here.
				// 4. Call UGamjeplayStatics::ApplyRadialDamageWithFalloff to cause damage, which results in Take Damage being called on the Victim, broadcast OnDamageDelegate
				// 5. In Lambda, Set DamageTypeValue to the damage received from the broadcast

				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
				{
					CombatInterface->GetOnDamageDelegate().AddLambda([&](float DamageAmount)
					{
						DamageTypeValue = DamageAmount;
					});
				}
				UGameplayStatics::ApplyRadialDamageWithFalloff(
					TargetAvatar,
					DamageTypeValue,
					0.f,
					UHAFAbilitySystemBlueprintLibrary::GetRadialDamageOrigin(EffectContextHandle),
					UHAFAbilitySystemBlueprintLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
					UHAFAbilitySystemBlueprintLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
					1.f,
					UDamageType::StaticClass(),
					TArray<AActor*>(),
					SourceAvatar,
					nullptr);
			}
		
			// If the source is resistant to the damage type, add the damage type's value to the damage.
			Damage += DamageTypeValue;
		}

		// PART TWO: Capture BLOCKCHANCE on Target and determine if there was a successful BLOCK
		float TargetBlockChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
		TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
		// If it was blocked, cut the damage by 50%.
		const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

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
}
