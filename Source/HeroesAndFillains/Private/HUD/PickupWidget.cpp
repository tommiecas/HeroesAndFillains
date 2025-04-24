// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/PickupWidget.h"
#include "Components/TextBlock.h"
#include "Weapons/Weapon.h"
#include "Weapons/WeaponTypes.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"



FString UPickupWidget::GetWeaponTypeDisplayName(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::EWT_AssaultRifle:
		return FString("Assault Rifle");
	case EWeaponType::EWT_RocketLauncher:
		return FString("Rocket Launcher");
	case EWeaponType::EWT_Pistol:
		return FString("Pistol");
	case EWeaponType::EWT_SubmachineGun:
		return FString("Submachine Gun");
	case EWeaponType::EWT_Shotgun:
		return FString("Shotgun");
	case EWeaponType::EWT_SniperRifle:
		return FString("Sniper Rifle");
	case EWeaponType::EWT_GrenadeLauncher:
		return FString("Grenade Launcher");
		// Add other weapon types here
	default:
		return FString("Unknown Weapon");
	}
}

void UPickupWidget::SetWeaponNameText(FString WeaponNameTextToDisplay, AWeaponFinal* InWeaponFinal)
{
	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(WeaponNameTextToDisplay));
		ShowWeaponFinalName(InWeaponFinal);
	}
}

void UPickupWidget::ShowWeaponFinalName(class AWeaponFinal* InWeaponFinal)
{
   if (InWeaponFinal == nullptr || InWeaponFinal->WeaponFinalTypeDisplayed == EWeaponFinalTypeDisplayed::EWFTD_None)
   {
       // Handle the null case, maybe log an error or set a default player name
       SetWeaponNameText(FString("Unknown Weapon"), InWeaponFinal);
       return;
   }

   FString WeaponName = InWeaponFinal->GetWeaponDisplayName(InWeaponFinal->WeaponFinalTypeDisplayed);
   SetWeaponNameText(WeaponName, InWeaponFinal);
}