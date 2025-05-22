// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MeleeInfoWidgetComponent.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UMeleeInfoWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon UI")
	void SetMeleeWeaponInfo(const FText& Name, const FText& History, const FText& Resistances, const FText& Weaknesses, float Damage);

	
protected:
	// Stored info to be used by the widget (e.g., for binding or in the Blueprint event)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeHistoryText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeResistancesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaknessesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	float MeleeDamageText;
};


