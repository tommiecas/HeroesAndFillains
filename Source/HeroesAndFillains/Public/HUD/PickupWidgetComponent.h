// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WeaponsFinal/WeaponsFinalTypes.h" // Ensure this header is included for EWeaponFinalType
#include "PickupWidgetComponent.generated.h"

/**
* 
*/
UCLASS()
class HEROESANDFILLAINS_API UPickupWidgetComponent : public UWidgetComponent
{
GENERATED_BODY()

public:
UPickupWidgetComponent();
UPROPERTY(meta = (BindWidget))
class UTextBlock* WeaponNameText;

FString GetWeaponFinalTypeDisplayName(EWeaponFinalType WeaponFinalType);
};
