// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "RangedInfoWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API URangedInfoWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon UI")
    void SetRangedWeaponInfo(const FText& Name, const FText& Description, const FText& Type, const FText& Rarity, float Damage);

protected:
    // Stored info to be used by the widget (e.g., for binding or in the Blueprint event)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
    FText WeaponNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
    FText WeaponDescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
    FText WeaponTypeText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
    FText WeaponRarityText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
    float WeaponDamageValue;
};


