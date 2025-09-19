#pragma once

UENUM(BlueprintType)
enum class EBattlePrepped : uint8
{
	EBP_Defenseless UMETA(DisplayName = "Has No Weapon"),
	EBP_ArmedOneHandedRangedWeapon UMETA(DisplayName = "Armed With an One-Handed Firearm"),
	EBP_ArmedTwoHandedRangedWeapon UMETA(DisplayName = "Armed With a Two-Handed Firearm"),
	EBP_ArmedOneHandedMeleeWeapon UMETA(DisplayName = "Armed With a One-Handed Melee Weapon"),
	EBP_ArmedTwoHandedMeleeWeapon UMETA(DisplayName = "Armed With a Two-Handed Melee Weapon"),
	EBP_ArmedOneHandedMajixWeapon UMETA(DisplayName = "Armed With a One-Handed Majix Spell"),
	EBP_ArmedTwoHandedMajixWeapon UMETA(DisplayName = "Armed WIth a Two-Handed Majix Spell"),
	
	EBP_Disarmed UMETA(DisplayName = "Equipped With a Weapon, But Currently Disarmed"),
	
	EBP_MAX UMETA(DisplayName = "DefaultMAX")
};