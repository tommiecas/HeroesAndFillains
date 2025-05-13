// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
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
enum class EFireTypeFinal : uint8
{
	EFTF_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFTF_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFTF_Shotgun UMETA(DisplayName = "Shotgun Weapon"),

	EFTF_MAX UMETA(DisplayName = "DefaultMAX")
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
	void ShowPickupAndWeaponInfoWidgets(bool bShowPickupAndWeaponInfoWidgets);
	virtual void Fire(const FVector& HitTarget);
	virtual void WeaponFinalDropped();
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
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class USoundCue* EquipSound;

	/*********************************************
	****                                      ****
	****    ENABLE OR DISABLE CUSTOM DEPTH    ****
	****                                      ****
	*********************************************/

	void EnableCustomDepth(bool bEnable);

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

	bool bDestroyWeaponFinal = false;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponFinalType WeaponFinalType;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EFireTypeFinal FireTypeFinal;

	/*************************************
	****                              ****	
	****    WEAPON PICK-UP EFFECTS    ****
	****                              ****
	*************************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldHover = true;

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
	
	/*****************************
	***                        ***
	***   WEAPON INFORMATION   ***
	***                        ***
	*****************************/

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* WeaponInfoWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* WeaponInfoWidget2;

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

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetA;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetB;
	
	UPROPERTY()
    class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnWeaponFinalStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

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

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY()
	class AFillainCharacter* FillainOwnerCharacter;
	
	UPROPERTY()
	class AFillainPlayerController* FillainOwnerController;

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	FText GetWeaponTypeText() const;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponFinalState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponFinalState WeaponFinalState;

	UFUNCTION()
	void OnRep_WeaponFinalState();

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
	
	UPROPERTY(EditAnywhere)
	ETeam Team;


public:
	void SetWeaponFinalState(EWeaponFinalState FinalState);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UWidgetComponent* GetPickupWidgetA() const { return PickupWidgetA; }
	FORCEINLINE UWidgetComponent* GetPickupWidgetB() const { return PickupWidgetB; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsWeaponFinalEmpty();
	bool IsWeaponFinalFull();
	FORCEINLINE EWeaponFinalType GetWeaponFinalType() const { return WeaponFinalType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE ETeam GetTeam() const { return Team; }





};
