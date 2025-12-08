// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFGameplayAbility.h"

#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystem/Abilities/HAFDamageGameplayAbility.h"
#include "GameMode/HaFGameMode.h"

FString UHAFGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Title>ABILITY NAME     LEVEL: </Title><Level>%d</Level>\n\n<Default>Majix Cost:</Default><MajixCost>##</MajixCost>\n<Default>Cooldown></Default><Cooldown>##</Cooldown>\n\n<Default>Does something with </Default><MoreThanBefore>this amount </MoreThanBefore><Default>of the things, which then do this other thing and deal </Default><Damage>this amount of</Damage><Default> damage with a chance to also </Default> <DurationalDamage> apply more damaging effects! </DurationalDamage>\n\n "), Level);
}

float UHAFGameplayAbility::GetAbilityCost(const FGameplayAttribute& Attribute) const
{
	if (!CostGameplayEffectClass) return 0.f;

	const UGameplayEffect* GE = CostGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	if (!GE) return 0.f;

	const float Level = GetSafeAbilityLevel();

	for (const FGameplayModifierInfo& Mod : GE->Modifiers)
	{
		if (Mod.Attribute == Attribute)
		{
			float OutMagnitude = 0.f;
			if (Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, OutMagnitude))
			{
				return FMath::Abs(OutMagnitude);
			}
		}
	}

	return 0.f;
}

FString UHAFGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>NextLevel: </Default><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UHAFGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level:</Default><Level> %d</Level>"), Level);
}

float UHAFGameplayAbility::GetMajixCost(float InLevel)
{
	float MajixCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (auto Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UHAFAttributeSet::GetMajixAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, MajixCost);
				break;
			}
		}
	}
	return MajixCost;
}

float UHAFGameplayAbility::GetCooldown(float InLevel)
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}

int32 UHAFGameplayAbility::GetSafeAbilityLevel() const
{
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Spec->Level;
	}

	return 1; // Safe fallback for UI / menus
}

float UHAFGameplayAbility::GetScalableModifierMagnitude(TSubclassOf<UGameplayEffect> GEClass,
	const FGameplayAttribute& Attribute, float AbilityLevel)
{
	if (!GEClass) return 0.f;

	const UGameplayEffect* GE = GEClass->GetDefaultObject<UGameplayEffect>();
	if (!GE) return 0.f;

	for (const FGameplayModifierInfo& Mod : GE->Modifiers)
	{
		if (Mod.Attribute == Attribute)
		{
			float OutMagnitude = 0.f;
			if (Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(AbilityLevel, OutMagnitude))
			{
				return OutMagnitude;
			}
		}
	}

	return 0.f;
}

float UHAFGameplayAbility::GetAbilityCooldown() const
{
	if (!CooldownGameplayEffectClass) return 0.f;

	const UGameplayEffect* GE = CooldownGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	if (!GE) return 0.f;

	const float Level = GetSafeAbilityLevel();

	for (const FGameplayModifierInfo& Mod : GE->Modifiers)
	{
		float OutMagnitude = 0.f;
		if (Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, OutMagnitude))
		{
			return OutMagnitude;
		}
	}

	return 0.f;
}

float UHAFGameplayAbility::CalculateAbilityCost(int32 CharacterLevel)
{
	return 0.f;
}
