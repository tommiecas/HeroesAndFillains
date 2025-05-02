// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/PickupWidgetComponent.h"
#include "RandomizedWeaponWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API URandomizedWeaponWidgetComponent : public UPickupWidgetComponent
{
	GENERATED_BODY()

public:
	URandomizedWeaponWidgetComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "WeaponInfo")
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, Category = "WeaponInfo")
	FString WeaponDescription;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponInfo")
	FString WeaponType;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponInfo")
	FString WeaponRarity;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponInfo")
	FString WeaponDamage;

	
};
