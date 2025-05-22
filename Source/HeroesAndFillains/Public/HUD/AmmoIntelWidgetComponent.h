// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AmmoIntelWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UAmmoIntelWidgetComponent : public UWidgetComponent

{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon UI")
	void SetWeaponAmmoIntel(const FText& Name, const FText& Weapon, const FText& Deliverable, float Amount, float Damage);
	
protected:
	// Stored info to be used by the widget (e.g., for binding or in the Blueprint event)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText AmmoNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText AmmoWeaponText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText AmmoDeliverableText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	float AmmoAmountText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	float AmmoDamageText;
};

