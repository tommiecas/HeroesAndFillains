// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInfoWidget.h"
#include "MeleeInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMeleeInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon UI")
	void SetMeleeWeaponInfo(const FText& Name, const FText& History, const FText& Resistances, const FText& Weaknesses, float Damage);

protected:
	// Stored info to be used by the widget (e.g., for binding or in the Blueprint event)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeHistoryText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeResistancesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeWeaknessesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	float MeleeDamageText;
};


