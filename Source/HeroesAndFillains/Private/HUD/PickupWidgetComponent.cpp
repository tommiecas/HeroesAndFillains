// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PickupWidgetComponent.h"
#include "Components/TextBlock.h"
#include "Weapons/WeaponTypes.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"

UPickupWidgetComponent::UPickupWidgetComponent()
{
	AWeaponBase* WeaponToBePickedUp = Cast<AWeaponBase>(GetOwner());
	if (WeaponToBePickedUp)
	{
		if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(WeaponToBePickedUp))
		{
			EMeleeType NoMeleeType = EMeleeType::EMT_None;
			ERangedType TypeOfRangedWeapon = RangedWeapon->GetRangedWeaponType();
			GetWeaponTypeDisplayName(TypeOfRangedWeapon, NoMeleeType);			
		}
		if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(WeaponToBePickedUp))
		{
			ERangedType NoRangedType = ERangedType::ERT_None;
			EMeleeType TypeOfMeleeWeapon = MeleeWeapon->GetMeleeWeaponType();
			GetWeaponTypeDisplayName(NoRangedType, TypeOfMeleeWeapon);			
		}
	}
}

FString UPickupWidgetComponent::GetWeaponTypeDisplayName(ERangedType RangedType, EMeleeType MeleeType)
{
	if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(GetOwner()))
	{
		switch (MeleeType)
		{
		case EMeleeType::EMT_ChaosSword:
			return FString("Chaos Sword");
		case EMeleeType::EMT_RubySword:
			return FString("Ruby Sword");
		case EMeleeType::EMT_SapphireSword:
			return FString("Sapphire Sword");
			// Add other weapon types here
		default:
			return FString("Unknown Weapon");
		}
	}
	else if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(GetOwner()))
	{
		switch (RangedType)
		{
		case ERangedType::ERT_AssaultRifle:
			return FString("Assault Rifle");
		case ERangedType::ERT_RocketLauncher:
			return FString("Rocket Launcher");
		case ERangedType::ERT_Pistol:
			return FString("Pistol");
		case ERangedType::ERT_SubmachineGun:
			return FString("Submachine Gun");
		case ERangedType::ERT_Shotgun:
			return FString("Shotgun");
		case ERangedType::ERT_SniperRifle:
			return FString("Sniper Rifle");
		case ERangedType::ERT_GrenadeLauncher:
			return FString("Grenade Launcher");
	
		default:
			return FString("Unknown Weapon");
		}
	}
	else return FString("Unknown Weapon");
}