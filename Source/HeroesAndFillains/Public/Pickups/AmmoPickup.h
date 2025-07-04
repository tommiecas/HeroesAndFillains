// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponTypes.h"
#include "Pickups/Pickup.h"
#include "AmmoPickup.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_None UMETA(DisplayName = "None"),

	EAT_ARAmmo UMETA(DisplayName = "ARAmmo"),
	EAT_Rockets UMETA(DisplayName = "Rockets"),
	EAT_Bullets UMETA(DisplayName = "Bullets"),
	EAT_Magazine UMETA(DisplayName = "Magazine"),
	EAT_Shells UMETA(DisplayName = "Shells"),
	EAT_SniperAmmo UMETA(DisplayName = "SniperAmmo"),
	EAT_LaunchedGrenades UMETA(DisplayName = "Grenades"),
	
	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};

class UPickupGearWidget;

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	AAmmoPickup();
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void SetAmmoPickupInformationText(UWidgetComponent* AmmoPickupWidgetComponent, AAmmoPickup* AmmoPickup);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Info")
	FString AmmoName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Info")
	FString AmmoWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Info")
	FString AmmoDeliverable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Info")
	FString AmmoAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Info")
	FString AmmoDamage;
	
	UPROPERTY(EditAnywhere)
	int32 AmountOfAmmoInside = 30;
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	EAmmoType AmmoType = EAmmoType::EAT_None;

	
	
protected:
	virtual void BeginPlay() override;

private:
	

public:
	FORCEINLINE EAmmoType GetAmmoPickupType() const { return AmmoType; }
};
