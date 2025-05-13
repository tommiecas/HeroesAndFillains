// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon UI")
    void SetWeaponInfo(const FText& Name, const FText& Description, const FText& Type, const FText& Rarity, float Damage);

protected:
    // Stored info to be used by the widget (e.g., for binding or in the Blueprint event)
    UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FText WeaponNameText;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FText WeaponDescriptionText;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FText WeaponTypeText;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FText WeaponRarityText;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float WeaponDamageValue;
};


