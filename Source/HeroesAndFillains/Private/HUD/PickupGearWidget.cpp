// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PickupGearWidget.h"

#include "HUD/PickupWidgetComponent.h"
#include "Components/TextBlock.h"
#include "Pickups/AmmoPickup.h"
#include "Pickups/PickupSpawnPoint.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"

UPickupGearWidget::UPickupGearWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UPickupGearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Safe to access owning component here
	PickupGearOwningComponent = Cast<UWidgetComponent>(GetOwningWidgetComponent());
}

/*
void UPickupGearWidget::DelineatePickupType()
{
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(OwningActor))
	{
		EAmmoType NoAmmoSpawnPointType = EAmmoType::EAT_None;
		EMeleeType NoMeleeType = EMeleeType::EMT_None;
		ERangedType TypeOfRangedWeapon = RangedWeapon->GetRangedWeaponType();
		GetPickupTypeDisplayName(TypeOfRangedWeapon, NoMeleeType, NoAmmoSpawnPointType);						
	}
	if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(OwningActor))
	{
		EAmmoType NoAmmoSpawnPointType = EAmmoType::EAT_None;
		ERangedType NoRangedType = ERangedType::ERT_None;
		EMeleeType TypeOfMeleeWeapon = MeleeWeapon->GetMeleeWeaponType();
		GetPickupTypeDisplayName(NoRangedType, TypeOfMeleeWeapon, NoAmmoSpawnPointType);			
	}
	if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(OwningActor))
	{
		ERangedType NoRangedType = ERangedType::ERT_None;
		EMeleeType NoMeleeType = EMeleeType::EMT_None;
		EAmmoType TypeOfAmmoPickup = AmmoPickup->GetAmmoPickupType();
		GetPickupTypeDisplayName(NoRangedType, NoMeleeType, TypeOfAmmoPickup);			
	}
}

FString UPickupGearWidget::GetPickupTypeDisplayName(ERangedType RangedType, EMeleeType MeleeType, EAmmoType AmmoType)
{
	if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(OwningActor))
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
	else if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(OwningActor))
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
	else if (AAmmoPickup* Pickup = Cast<AAmmoPickup>(OwningActor))
	{
		switch (AmmoType)
		{
		case EAmmoType::EAT_ARAmmo:
			return FString("Assault Rifle Ammo");
		case EAmmoType::EAT_Rockets:
			return FString("Rockets");
		case EAmmoType::EAT_Bullets:
			return FString("Bullets");
		case EAmmoType::EAT_Magazine:
			return FString("Magazine");
		case EAmmoType::EAT_Shells:
			return FString("Shells");
		case EAmmoType::EAT_SniperAmmo:
			return FString("Sniper Ammo");
		case EAmmoType::EAT_LaunchedGrenades:
			return FString("Launched Grenades");

		default:
			return FString("Unknown Ammunition");
		}
	}
	else return FString("Unknown Gear");
}*/