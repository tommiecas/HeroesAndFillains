// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PickupWidgetComponent.h"
#include "Components/TextBlock.h"
#include "Weapons/WeaponTypes.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"

UPickupWidgetComponent::UPickupWidgetComponent()
{
	AWeaponFinal* WeaponToBePickedUp = Cast<AWeaponFinal>(GetOwner());
	if (WeaponToBePickedUp)
	{
		EWeaponFinalType TypeOfWeaponFinalToPickUp = WeaponToBePickedUp->GetWeaponFinalType();
		GetWeaponFinalTypeDisplayName(TypeOfWeaponFinalToPickUp);
	}
}

FString UPickupWidgetComponent::GetWeaponFinalTypeDisplayName(EWeaponFinalType WeaponType)
{
	switch (WeaponType)
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
	case EWeaponFinalType::EWFT_Sword:
		return FString("Sword");
		// Add other weapon types here
	default:
		return FString("Unknown Weapon");
	}
}



