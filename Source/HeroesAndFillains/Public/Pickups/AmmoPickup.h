// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Pickups/Pickup.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "AmmoPickup.generated.h"


class ARangedWeapon;
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

	UPROPERTY()
	ARangedWeapon* RangedWeapon;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UPickupGearWidget> PickupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UItemInfoWidgetBase> InfoWidgetClass;

	UPROPERTY()
	TObjectPtr<UHAFUserWidget> PickupWidget;

	UPROPERTY()
	TObjectPtr<UHAFUserWidget> InfoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* HoveringLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* HoveringDecal;


	
protected:
	virtual void BeginPlay() override;

private:
	

public:
	FORCEINLINE EAmmoType GetAmmoPickupType() const { return RangedWeapon->GetAmmoType(); }
};
