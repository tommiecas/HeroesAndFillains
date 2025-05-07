// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/PickupWidget.h"
#include "Components/TextBlock.h"
#include "Weapons/Weapon.h"
#include "Weapons/WeaponTypes.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"



FString UPickupWidget::GetWeaponTypeDisplayName(EWeaponFinalType WeaponFinalType)
{
	switch (WeaponFinalType)
	{
	case EWeaponFinalType::EWFT_AssaultRifle:
		return FString("Assault Rifle");
	case EWeaponFinalType::EWFT_RocketLauncher:
		return FString("Rocket Launcher");
	case EWeaponFinalType::EWFT_Pistol:
		return FString("Pistol");
	case EWeaponFinalType::EWFT_SubmachineGun:
		return FString("Submachine Gun");
	case EWeaponFinalType::EWFT_Shotgun:
		return FString("Shotgun");
	case EWeaponFinalType::EWFT_SniperRifle:
		return FString("Sniper Rifle");
	case EWeaponFinalType::EWFT_GrenadeLauncher:
		return FString("Grenade Launcher");
		// Add other weapon types here
	default:
		return FString("Unknown Weapon");
	}
}

