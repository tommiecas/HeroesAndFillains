// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/WeaponTypes.h" // Ensure this header is included for Weapon Types
#include "Pickups/AmmoPickup.h"
#include "Pickups/PickupSpawnPoint.h"

#include "PickupGearWidget.generated.h"

class APickupSpawnPoint;
/**
* 
*/
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UPickupGearWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class UWidgetComponent* OwningWidgetComponent;
	
	/* void DelineatePickupType();

	UPROPERTY(BlueprintReadOnly, Category = "Pickup", meta = (ExposeOnSpawn = true))
	AActor* OwningActor;

	FString GetPickupTypeDisplayName(ERangedType RangedType, EMeleeType MeleeType, EAmmoType AmmoType);
	
*/
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnimation;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnimation;


};
