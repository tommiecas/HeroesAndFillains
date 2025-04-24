// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapons/WeaponTypes.h"
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

	class FString GetWeaponTypeDisplayName(EWeaponType WeaponType);

	void SetWeaponNameText(FString WeaponNameTextToDisplay, AWeaponFinal* InWeaponFinal);

	UFUNCTION(BlueprintCallable)
	void ShowWeaponFinalName(class AWeaponFinal* InWeaponFinal);


	
};
