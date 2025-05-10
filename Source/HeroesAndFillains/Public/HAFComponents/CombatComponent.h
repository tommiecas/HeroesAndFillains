// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/FillainHUD.h"
#include "Weapons/WeaponTypes.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/CombatState.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "WeaponsFinal/WeaponFinal.h"

#include "CombatComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEROESANDFILLAINS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class AFillainCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeaponFinal(class AWeaponFinal* WeaponFinalToEquip);
	void SwapWeapons();
	void Reload();
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeaponFinal)
	AWeaponFinal* EquippedWeaponFinal;

	UFUNCTION()
	void OnRep_EquippedWeaponFinal();

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeaponFinal)
	AWeaponFinal* SecondaryWeaponFinal;

    UPROPERTY()
    EWeaponFinalType WeaponFinalType;

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponFinalType WeaponFinalType, int32 AmmoAmount);

	bool bLocallyReloading = false;

	FVector HitTarget;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_SecondaryWeaponFinal();

	
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();

	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable /*, WithValidation */ )
	void ServerFire(const FVector_NetQuantize& TraceHitTarget /*, float FireDelay*/ );

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);


	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReloading();

	void HandleReload();
	int32 AmountToReload();

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileFinal> GrenadeFinalClass;

	void DropEquippedWeaponFinal();

	void AttachActorToRightHand(class AActor* ActorToAttach);
	void AttachActorToLeftHand(class AActor* ActorToAttach);
	void AttachSwordToLeftHand(AWeaponFinal* Sword);
	void AttachActorToBackpack(class AActor* ActorToAttach);	
	void UpdateCarriedAmmo();

	void PlayWeaponFinalEquipSound(AWeaponFinal* WeaponFinalToEquip);

	void ReloadEmptyWeaponFinal();

	void ShowAttachedGrenade(bool bShowGrenade); 

	void UpdateHUDGrenades();

	void EquipPrimaryWeaponFinal(AWeaponFinal* WeaponFinalToEquip);
	void EquipSecondaryWeaponFinal(AWeaponFinal* WeaponFinalToEquip);

private:
	UPROPERTY()
	class AFillainCharacter* Character;
	
	UPROPERTY()
	class AFillainPlayerController* Controller;
	
	UPROPERTY()
	class AFillainHUD* HUD;

	

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	bool bIsFireButtonPressed;

	/*****************************
	***                        ***
	***   HUD AND CROSSHAIRS   ***
	***                        ***
	*****************************/

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FHUDPackage HUDPackage;

	/*************************
	***                    ***
	***   AIMING AND FOV   ***
	***                    ***
	* ***********************/

	float DefaultFOV;
	//field of view when not aiming; set to the camera's base FOV in BeginPlay

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*************************
	***                    ***
	***   Automatic Fire   ***
	***                    ***
	*************************/
	
	FTimerHandle FireTimer;

	bool bCanGunFire = true;
	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	/****************
	***    AMMO   ***
	****************/

	//carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponFinalType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	void InitializeCarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 4;

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;

	UPROPERTY(ReplicatedUsing = OnRep_WieldingTheSword)
	bool bWieldingTheSword = false;

	UFUNCTION()
	void OnRep_WieldingTheSword();

	UPROPERTY()
	AWeaponFinal* TheSword;

public:	
	FORCEINLINE bool IsAiming() const { return bAiming; }
	FORCEINLINE EWeaponFinalType GetWeaponFinalType() const { return WeaponFinalType; }
	FORCEINLINE int32 GetGrenades() const { return Grenades; }	
	bool ShouldSwapWeapons();
	
		
};
