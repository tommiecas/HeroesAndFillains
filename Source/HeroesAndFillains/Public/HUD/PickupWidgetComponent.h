// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/WeaponTypes.h" // Ensure this header is included for Weapon Types
#include "PickupWidgetComponent.generated.h"

/**
* 
*/
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UPickupWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UPickupWidgetComponent();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponNameText;

	FString GetWeaponTypeDisplayName(ERangedType RangedType, EMeleeType MeleeType);
};
