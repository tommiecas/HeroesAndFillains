// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

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
	FGameplayTag Attributes_Secondary_Charm;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ShieldRegeneration;
	FGameplayTag Attributes_Secondary_StaminaRegeneration;
	FGameplayTag Attributes_Secondary_MajixRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxShield;
	FGameplayTag Attributes_Secondary_MaxStamina;
	FGameplayTag Attributes_Secondary_MaxMajix;

	FGameplayTag Attributes_Invisible_DexterityAgilityFlexibility;
protected:

private:
	static FHAFGameplayTags GameplayTags;
	
};
