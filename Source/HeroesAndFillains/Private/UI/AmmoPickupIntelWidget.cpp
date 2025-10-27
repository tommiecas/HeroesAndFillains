

// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AmmoPickupIntelWidget.h"

#include "Components/WidgetComponent.h"
#include "Pickups/AmmoPickup.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Components/TextBlock.h"


void UAmmoPickupIntelWidget::UpdateAmmoPickupInformation(const FString& Name, const FString& MatchingWeapon,
	const FString& Deliverable, const FString& Amount, float Damage)
{
	if (AmmoNameText)
		AmmoNameText->SetText(FText::FromString(Name));

	if (AmmoWeaponText)
		AmmoWeaponText->SetText(FText::FromString(MatchingWeapon));

	if (AmmoDeliverableText)
		AmmoDeliverableText->SetText(FText::FromString(Deliverable));

	if (AmmoAmountText)
		AmmoAmountText->SetText(FText::FromString(Amount));

	if (AmmoDamageText)
		AmmoDamageText->SetText(FText::AsNumber(Damage));
}

void UAmmoPickupIntelWidget::DelineatePickupType()
{
	UWidgetComponent* OwningComp = Cast<UWidgetComponent>(GetOuter());
	if (OwningComp)
	{
		AActor* OwnerActor = OwningComp->GetOwner();
		if (OwnerActor)
		{
			if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(OwnerActor))
			{
				EAmmoType NoAmmoSpawnPointType = EAmmoType::EAT_None;
				EMeleeType NoMeleeType = EMeleeType::EMT_None;
				ERangedType TypeOfRangedWeapon = Ranged->GetRangedType();
				GetPickupTypeDisplayName(TypeOfRangedWeapon, NoMeleeType, NoAmmoSpawnPointType);						
			}
			if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(OwnerActor))
			{
				EAmmoType NoAmmoSpawnPointType = EAmmoType::EAT_None;
				ERangedType NoRangedType = ERangedType::ERT_None;
				EMeleeType TypeOfMeleeWeapon = Melee->GetMeleeWeaponType();
				GetPickupTypeDisplayName(NoRangedType, TypeOfMeleeWeapon, NoAmmoSpawnPointType);			
			}
			if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(OwnerActor))
			{
				ERangedType NoRangedType = ERangedType::ERT_None;
				EMeleeType NoMeleeType = EMeleeType::EMT_None;
				EAmmoType TypeOfAmmoPickup = AmmoPickup->GetAmmoPickupType();
				GetPickupTypeDisplayName(NoRangedType, NoMeleeType, TypeOfAmmoPickup);			
			}
		}
	}
}

FString UAmmoPickupIntelWidget::GetPickupTypeDisplayName(ERangedType RangedWeaponType, EMeleeType MeleeWeaponType, EAmmoType AmmoPickupType)
{
	UWidgetComponent* OwningComp = Cast<UWidgetComponent>(GetOuter());
	if (OwningComp)
	{
		AActor* OwnerActor = OwningComp->GetOwner();
		if (OwnerActor)
		{
			if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(OwnerActor))
			{
				switch (MeleeWeaponType)
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
			if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(OwnerActor))
			{
				switch (RangedWeaponType)
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
			if (AAmmoPickup* Pickup = Cast<AAmmoPickup>(OwnerActor))
			{
				switch (AmmoPickupType)
				{
				case EAmmoType::EAT_ARAmmo:
					return FString("Assault Rifle Ammo");
				case EAmmoType::EAT_Rockets:
					return FString("Rockets");
				case EAmmoType::EAT_Bullets:
					return FString("Bullets");
				case EAmmoType::EAT_Magazines:
					return FString("Magazines");
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
			return FString("Unknown Gear");
		}
	}
	return FString("Unknown Pickup");
}
