// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_StaminaCost);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_StaminaRegen);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_MajixCost);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_MajixRegen);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_ShieldRegen);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_VitalEffects_HealthRegen);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Damage_Shield);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Damage_Health);

/**
 * HAFGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */

struct FHAFGameplayTags
{
public:
	static const FHAFGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Dexterity;
	FGameplayTag Attributes_Primary_Marksmanship;
	FGameplayTag Attributes_Primary_Wisdom;
	FGameplayTag Attributes_Primary_Vigor;
	FGameplayTag Attributes_Primary_Charisma;
	
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_Agility;
	FGameplayTag Attributes_Secondary_Flexibility;
	FGameplayTag Attributes_Secondary_Purity;
	FGameplayTag Attributes_Secondary_Corruptibility;
	FGameplayTag Attributes_Secondary_Intuition;
	FGameplayTag Attributes_Secondary_Vision;
	FGameplayTag Attributes_Secondary_Speed;
	FGameplayTag Attributes_Secondary_Charm;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ShieldRegeneration;
	FGameplayTag Attributes_Secondary_StaminaRegeneration;
	FGameplayTag Attributes_Secondary_MajixRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxShield;
	FGameplayTag Attributes_Secondary_MaxStamina;
	FGameplayTag Attributes_Secondary_MaxMajix;

	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Shield;
	FGameplayTag Attributes_Vital_Stamina;
	FGameplayTag Attributes_Vital_Majix;

	FGameplayTag Attributes_Invisible_DexterityAgilityFlexibility;

	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_ChaosMajix;
	FGameplayTag Attributes_Resistance_MeleeAttacks;
	FGameplayTag Attributes_Resistance_RuleOfOrder;
	FGameplayTag Attributes_Resistance_Ice;
	FGameplayTag Attributes_Resistance_Toxicity;
	FGameplayTag Attributes_Resistance_Stun;
	
	FGameplayTag InputTag_LeftMouseButtonOrGamepadShoulder;
	FGameplayTag InputTag_RightMouseButtonOrGamepadShoulder;
	FGameplayTag InputTag_1OrDPadUp;
	FGameplayTag InputTag_2OrDPadDown;
	FGameplayTag InputTag_3OrDPadLeft;
	FGameplayTag InputTag_4OrDPadRight;

	FGameplayTag Damage;
	FGameplayTag Damage_IncomingDamage;
	FGameplayTag Damage_Burn;
	FGameplayTag Damage_Shock;
	FGameplayTag Damage_CorruptChaos;
	FGameplayTag Damage_Physical;
	FGameplayTag Damage_PureOrder;
	FGameplayTag Damage_Frostbite;
	FGameplayTag Damage_Poison;
	FGameplayTag Damage_Paralysis;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	FGameplayTag Effects_HitReact;

	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBolt;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftFoot;
	FGameplayTag CombatSocket_RightFoot;
	FGameplayTag CombatSocket_BothHands;
	FGameplayTag CombatSocket_BothFeet;
	FGameplayTag CombatSocket_HandsAndFeet;
	FGameplayTag CombatSocket_WeaponAndFeet;
	FGameplayTag CombatSocket_WeaponAndHands;
	FGameplayTag CombatSocket_WeaponAndHandsAndFeet;
	FGameplayTag CombatSocket_Teeth;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	FGameplayTag Montage_Attack_5;
	FGameplayTag Montage_Attack_6;
	FGameplayTag Montage_Attack_7;
	FGameplayTag Montage_Attack_8;
	FGameplayTag Montage_Attack_9;
	FGameplayTag Montage_Attack_10;
	FGameplayTag Montage_Attack_11;
	FGameplayTag Montage_Attack_12;
	FGameplayTag Montage_Attack_13;
	FGameplayTag Montage_Attack_14;
	FGameplayTag Montage_Attack_15;
	FGameplayTag Montage_Attack_16;
	FGameplayTag Montage_Attack_17;
	FGameplayTag Montage_Attack_18;
	FGameplayTag Montage_Attack_19;
	FGameplayTag Montage_Attack_20;
	
protected:

private:
	static FHAFGameplayTags GameplayTags;
	
};
