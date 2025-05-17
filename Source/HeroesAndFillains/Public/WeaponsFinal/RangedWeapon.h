// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Actor.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "RangedWeapon.generated.h"

UENUM(BlueprintType)
enum class ERangedType : uint8
{
	ERT_None UMETA(DisplayName = "None"),

	ERT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	ERT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	ERT_Pistol UMETA(DisplayName = "Pistol"),
	ERT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	ERT_Shotgun UMETA(DisplayName = "Shotgun"),
	ERT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	ERT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	ERT_Sword UMETA(DisplayName = "Sword"),
	
	ERT_MAX UMETA(DisplayName = "DefaultMAX")
};

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
	virtual void ShowPickupAndInfoWidgets(bool bShowPickupAndInfoWidgets) override;
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
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	ERangedType RangedWeaponType;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EFireType FireType;
	
	/*****************************
	***                        ***
	***   WEAPON INFORMATION   ***
	***                        ***
	*****************************/

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class URangedInfoWidgetComponent* RangedInfoWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	URangedInfoWidgetComponent* RangedInfoWidget2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText WeaponDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText WeaponRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	float WeaponDamage;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnEquipped() override;
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
	FText GetRangedWeaponTypeText() const;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	ERangedType RangedType = ERangedType::ERT_None;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasingFinal> CasingFinalClass;

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
	
	

public:
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsRangedWeaponEmpty();
	bool IsRangedWeaponFull();
	FORCEINLINE ERangedType GetRangedWeaponType() const { return RangedWeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	





};


