// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class HEROESANDFILLAINS_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	class USphereComponent* PickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* PickupHoverLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* PickupHoverDecal;

	// Floating hover parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldPickupHover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverAmplitude = 20.f; // How far it moves up/down (units)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverSpeed = 2.f; // How fast it oscillates



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldPickupFloatSpin = true;

	/*****************************
	***                        ***
	***   PICKUP INFORMATION   ***
	***                        ***
	*****************************/

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* AmmoInfoWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* AmmoInfoWidget2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText AmmoName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText AmmoDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText AmmoWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText AmmoRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	float AmmoAmount;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetA;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetB;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup* SpawnedPickup;

	void SpawnPickup();
	void SpawnPickupTimerFinished();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

private:
	FTimerHandle SpawnPickupTimer;
	
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;


	

};
