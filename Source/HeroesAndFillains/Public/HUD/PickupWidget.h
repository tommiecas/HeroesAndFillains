// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "PickupWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UPickupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponNameText;

	FString GetWeaponTypeDisplayName(EWeaponFinalType WeaponFinalType);

	
};
