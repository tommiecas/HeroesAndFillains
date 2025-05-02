// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/RandomizedWeaponWidgetComponent.h"

URandomizedWeaponWidgetComponent::URandomizedWeaponWidgetComponent(const FObjectInitializer& ObjectInitializer)
{
	WeaponName = TEXT("Default Weapon");
	WeaponDescription = TEXT("This is a default weapon description.");
	WeaponType = TEXT("Default Type");
	WeaponRarity = TEXT("Common");
	WeaponDamage = TEXT("10");
}
