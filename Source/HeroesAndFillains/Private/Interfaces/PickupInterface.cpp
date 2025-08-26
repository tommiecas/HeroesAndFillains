// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PickupInterface.h"

// Add default functionality here for any IPickupInterface functions that are not pure virtual.
void IPickupInterface::SetOverlappingItem(class APrePackagedPCPickupItem* Item)
{
}

void IPickupInterface::SetOverlappingWeapon(class AWeaponBase* Weapon)
{
}

void IPickupInterface::AddSoulsGatheredToTotalSouls(class ASoul* Soul)
{
}

void IPickupInterface::AddGoldAcquiredToTotalGold(class ATreasure* Treasure)
{
}
