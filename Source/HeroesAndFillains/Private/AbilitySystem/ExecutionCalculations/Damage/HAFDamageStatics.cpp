// HAFDamageStatics.cpp
#include "AbilitySystem/ExecutionCalculations/HAFDamageStatics.h"
#include "AbilitySystem/HAFAttributeSet.h"

HAFDamageStatics::HAFDamageStatics()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Armor, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ArmorPenetration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, BlockChance, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitChance, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitDamage, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitResistance, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Agility, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Flexibility, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Purity, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Corruptibility, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Intuition, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Vision, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Charm, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Speed, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, HealthRegeneration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ShieldRegeneration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, StaminaRegeneration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MajixRegeneration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxHealth, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxShield, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxStamina, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxMajix, Source, false);
	
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Fireproof, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Shockproof, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ChaosIncorruptible, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Invulnerability, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, HeartOfDarkness, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ThermalRadiation, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Immunity, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Unstoppable, Target, false);

	DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, IncomingDamage, Target, false);

	const FHAFGameplayTags& Tags = FHAFGameplayTags::Get();
	
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fireproof, FireproofDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Shockproof, ShockproofDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_ChaosIncorruptibility, ChaosIncorruptibleDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Invulnerability, InvulnerabilityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_HeartOfDarkness, HeartOfDarknessDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_ThermalRadiation, ThermalRadiationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Immunity, ImmunityDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Unstoppable, UnstoppableDef);
}
