// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "GameFramework/Actor.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "Weapons/WeaponTypes.h"
#include "RangedWeapon.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HEROESANDFILLAINS_API ARangedWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	ARangedWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	virtual void Fire(const FVector& HitTarget);
	
	void AddAmmo(int32 AmmoToAdd);
	FVector TraceEndWithScatter(const FVector& HitTarget);

	
	/*********************************************
	***                                        ***
	***   TEXTURES FOR THE WEAPON CROSSHAIRS   ***
	***                                        ***
	*********************************************/

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	/**********************************
	***                             ***
	***   ZOOMED FOV WHILE AIMING   ***
	***                             ***
	**********************************/

	UPROPERTY (EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/*************************
	***                    ***
	***   AUTOMATIC FIRE   ***
    ***                    ***
	*************************/

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bIsWeaponAutomatic = true;

	/*************************************
	****                              ****
	****    TRACE END WITH SCATTER    ****
	****                              ****
	*************************************/

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	bool bDestroyWeapon = false;

	UFUNCTION()
	void SetEquippedRangedWeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	ERangedType RangedType;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EFireType FireType;

	
protected:
	virtual void BeginPlay() override;
	virtual void OnEquippedOneHanded() override;
	virtual void OnEquippedTwoHanded() override;
	virtual void OnDropped() override;
	virtual void OnEquippedSecondary() override;
	
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult) override;
	
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	
	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;
	
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
	
	
	
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	ERangedType RangedTypeOfWeapon = ERangedType::ERT_None;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;



	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRoundOfAmmo();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	// The number of unprocessed server requests for Ammo.
	// Incremented in SpendRound, decremented in ClientUpdateAmmo.
	int32 Sequence = 0;

	UPROPERTY(EditAnywhere)
	AFillainCharacter* Fillain;

	UPROPERTY()
	UItemInfoWidgetBase* ItemInfoWidget;

public:
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsRangedWeaponEmpty();
	bool IsRangedWeaponFull();
	FORCEINLINE ERangedType GetRangedType() const { return RangedTypeOfWeapon; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE UItemInfoWidgetBase* GetItemInfoWidget() const { return ItemInfoWidget; }





};


