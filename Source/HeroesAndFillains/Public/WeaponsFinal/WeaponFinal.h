// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "WeaponFinal.generated.h"

UENUM(BlueprintType)
enum class EWeaponFinalState : uint8
{
	EWFS_Initial UMETA(DisplayName = "Initial State"),
	EWFS_Equipped UMETA(DisplayName = "Equipped State"),
	EWFS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary State"),
	EWFS_Dropped UMETA(DisplayName = "Dropped State"),

	EWFS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponFinalTypeDisplayed : uint8
{
	EWFTD_None UMETA(DisplayName = "None"),

	EWFTD_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWFTD_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWFTD_Pistol UMETA(DisplayName = "Pistol"),
	EWFTD_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWFTD_Shotgun UMETA(DisplayName = "Shotgun"),
	EWFTD_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWFTD_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	EWFTD_Sword UMETA(DisplayName = "Sword"),

	EWFTD_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HEROESANDFILLAINS_API AWeaponFinal : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponFinal();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void EnableCustomDepth(bool bEnable);


	void ShowPickupAndNameWidgets(bool bShowPickupAndNameWidgets);
	virtual void Fire(const FVector& HitTarget);
	FString GetWeaponDisplayName(EWeaponFinalTypeDisplayed DisplayNameType);
	void WeaponFinalDropped();

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

	UPROPERTY(ReplicatedUsing = OnRep_WeaponFinalTypeDisplayed, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponFinalTypeDisplayed WeaponFinalTypeDisplayed;

	void AddAmmo(int32 AmmoToAdd);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class USoundCue* EquipSound;

	bool bDestroyWeaponFinal = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	FVector TraceEndWithScatter(const FVector& HitTarget);


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponFinalState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponFinalState WeaponFinalState;

	

	UFUNCTION()
	void OnRep_WeaponFinalState();

	UFUNCTION()
	void OnRep_WeaponFinalTypeDisplayed();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetA;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetB;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UPickupWidgetComponent* NameWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UPickupWidgetComponent* NameWidget2;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasingFinal> CasingFinalClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRoundOfAmmo();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	class AFillainCharacter* FillainOwnerCharacter;

	UPROPERTY()
	class AFillainPlayerController* FillainOwnerController;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponFinalType WeaponFinalType;

public:
	void SetWeaponFinalState(EWeaponFinalState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsWeaponFinalEmpty();
	bool IsWeaponFinalFull();
	FORCEINLINE EWeaponFinalType GetWeaponFinalType() const { return WeaponFinalType; }
	FORCEINLINE EWeaponFinalTypeDisplayed GetWeaponFinalTypeDisplayed() const { return WeaponFinalTypeDisplayed; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE float GetDamage() const { return Damage; }





};
