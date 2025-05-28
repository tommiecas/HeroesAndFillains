// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfoWidgetBase.h"
#include "Pickups/AmmoPickup.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "AmmoPickupIntelWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UAmmoPickupIntelWidget : public UItemInfoWidgetBase
{
	GENERATED_BODY()

public:
	void UpdateAmmoPickupInformation(const FString& Name, const FString& MatchingWeapon, const FString& Deliverable, const FString& Amount, float Damage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoWeaponText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoDeliverableText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoAmountText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoDamageText;

	void DelineatePickupType();
	
	FString GetPickupTypeDisplayName(ERangedType RangedType, EMeleeType MeleeType, EAmmoType AmmoType);
};
