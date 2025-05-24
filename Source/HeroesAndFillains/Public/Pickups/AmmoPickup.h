// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Weapons/WeaponTypes.h"
#include "AmmoPickup.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_None UMETA(DisplayName = "None"),

	EAT_ARAmmo UMETA(DisplayName = "Ammo"),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	TSubclassOf<class UAmmoPickupIntelWidget> WeaponAmmoIntelWidgetClass;
	
	void ShowPickupAndInfoWidgets(bool bShow);
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	ERangedType RangedType;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	class USphereComponent* AmmoPickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* AmmoPickupHoverLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* AmmoPickupHoverDecal;

	UPROPERTY()
	EAmmoType AmmoType = EAmmoType::EAT_None;
	
	// Floating hover parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldPickupHover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverAmplitude = 20.f; // How far it moves up/down (units)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverSpeed = 2.f; // How fast it oscillates

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* HoverLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* HoverDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldFloatSpin = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldPickupFloatSpin = true;

	/*****************************
	***                        ***
	***   PICKUP INFORMATION   ***
	***                        ***
	*****************************/
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup Info")
	FString AmmoNameString;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Info")
	FString AmmoWeaponString;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Info")
	FString AmmoDeliverableString;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Info")
	FString AmmoAmountString;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Info")
	float AmmoDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	class UWidgetComponent* PickupGearWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* PickupGearWidgetComponentB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UPickupGearWidget> PickupGearWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ItemInfoWidgetClass;

	UPROPERTY()
	class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	EAmmoType AmmoSpawnPointType;

	UPROPERTY(EditAnywhere, Category = "UI Properties")
	UAmmoPickupIntelWidget* AmmoPickupIntelWidget;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Sine Parameters")
	float Amplitude = 0.25f;
	
	UPROPERTY(EditAnywhere, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
	UFUNCTION(BlueprintPure)
	float TransformedSin() const;
	
	UFUNCTION(BlueprintPure)
	float TransformedCos() const;

	

public:
	FORCEINLINE FString GetAmmoNameText() const { return AmmoNameString; }
	FORCEINLINE FString GetAmmoWeaponText() const { return AmmoWeaponString; }
	FORCEINLINE FString GetAmmoDeliverableText() const { return AmmoDeliverableString; }
	FORCEINLINE FString GetAmmoAmountText() const { return AmmoAmountString; }
	FORCEINLINE float GetAmmoDamageText() const { return AmmoDamage; }
	FORCEINLINE EAmmoType GetAmmoPickupType() const { return AmmoSpawnPointType; }
	FORCEINLINE UWidgetComponent* GetItemInfoWidgetComponentA() const { return ItemInfoWidgetComponentA; }
	FORCEINLINE UWidgetComponent* GetItemInfoWidgetComponentB() const { return ItemInfoWidgetComponentB; }
	FORCEINLINE UAmmoPickupIntelWidget* GetAmmoPickupIntelWidget() const { return AmmoPickupIntelWidget; }
	
};
