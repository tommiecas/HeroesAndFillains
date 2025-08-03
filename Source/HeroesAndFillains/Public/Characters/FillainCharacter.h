// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "HUD/OverheadWidget.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Engine/DamageEvents.h"
#include "GameMode/LobbyGameMode.h"
#include "HAFComponents/CombatComponent.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Interfaces/PickupInterface.h"
#include "FillainCharacter.generated.h"

class UGameplayEffect;
class AWeaponBase;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FInputActionInstance;
class UWidgetComponent;
class UCombatComponent;
class UAnimMontage;
class UCurveFloat;
class UTimelineComponent;
class UCurveFloat;
class UCurveLinearColor;
class UCurveVector;
class UDamageType;
class AFillainPlayerController;
class AHAFPlayerState;
class ALobbyGameMode;
class AProjectileFinal;
class UBoxComponent;

UENUM(BlueprintType)
enum class EBattlePrepped : uint8
{
	EBP_Defenseless UMETA(DisplayName = "Has No Weapon"),
	EBP_Armed UMETA(DisplayName = "Armed With an Equipped Weapon"),
	EBP_Disarmed UMETA(DisplayName = "Equipped With a Weapon, But Currently Disarmed"),
	
	EBP_MAX UMETA(DisplayName = "DefaultMAX")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLeavesGame);

UCLASS()
class HEROESANDFILLAINS_API AFillainCharacter : public ABaseCharacter, public IInteractWithCrosshairsInterface, public IPickupInterface
{
	GENERATED_BODY()

public:
	AFillainCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void DirectionalHitReact(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AddSoulsGatheredToTotalSouls(class ASoul* Soul) override;
	virtual void AddGoldAcquiredToTotalGold(class ATreasure* Treasure) override;


	// virtual void Restart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EquipOneHandedRangedWeapon(AWeaponBase* Weapon);
	void EquipTwoHandedRangedWeapon(AWeaponBase* Weapon);
	void EquipOneHandedMeleeWeapon(AWeaponBase* Weapon);
	void EquipTwoHandedMeleeWeapon(AWeaponBase* Weapon);
	void EquipWeapon(AWeaponBase* Weapon);
	void ActivateCombatCharacter();
	void InitializeBuffProperties();
	void ConfigureLagCompensation();
	virtual void OnRep_PlayerState() override;
	void InitializeAbilityActorInfo();
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	bool IsWeaponASword();
	bool PlayMeleeMontageForMeleeWeapons();
	void PlayRangedAnimationsForRangedWeapons(bool bAiming);
	void AssignTypeOfRangedWeapon(AWeaponBase* Weapon, FName SectionName);
	bool IsPlayerWeaponlessAndUnableToCombat();
	virtual void OnRep_ReplicatedMovement() override;
	void Eliminate(bool bPlayerLeftGame);
	void HideSniperScope();
	void ShowSniperScope();
	void StartDissolveEffect();
	void DisableAllComponents();
	void SpawnEliminationBotEffect();
	void PlayEliminationSound();
	void DestroyCrown();
	// void FinishElimination();
	virtual void Destroyed() override;
	void HideAttachedGrenade();
	// void OnFillainDying(AFillainCharacter* InstigatorFillain, AFillainCharacter* DyingFillain, class AFillainPlayerController* InstigatorController);
	void UpdateHUDAmmo();
	void SwitchWeapon(AWeaponBase* NewWeapon);
	ARangedWeapon* EquippedWeaponIsARangedWeapon();
	AMeleeWeapon* EquippedWeaponIsAMeleeWeapon();


	bool WeaponIsUnclaimedFirearm(ARangedWeapon* Ranged);
	bool WeaponIsUnclaimedMeleeWeapon(AMeleeWeapon* Melee);

	UFUNCTION(BlueprintCallable)
	bool IsUsingGamepad() const;


	UPROPERTY(VisibleAnywhere, Category = Combat)
	EBattlePrepped BattlePrepped = EBattlePrepped::EBP_Defenseless;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AMeleeWeapon* AcquiredMeleeWeapon;

	UPROPERTY()
	AFillainPlayerController* FillainPlayerController;

	UPROPERTY()
	AFillainPlayerController* VictimController;

	UPROPERTY()
	class AHAFPlayerState* HAFPlayerState;

	UPROPERTY()
	AFillainCharacter* VictimCharacter = nullptr;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminate(bool bPlayerLeftGame);
	
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	/******************************
	****    HAF COMPONENTS    *****
	******************************/
	
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ULagCompensationComponent* LagCompensation;

	void CacheDamageParameters(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	void ResetCachedDamageParameters();

	/************
	** Jumping **
	************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	virtual void Jump() override;

	/* UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit(); */


	/******************
	** PLAY MONTAGES **
	******************/
	void PlayFireMontage(bool bAiming);
	void PlayEliminatedMontage();
	virtual void PlayHitReactMontage(const FName& SectionName) override;
	void PlayReloadingMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();
	virtual int32 PlayMeleeAttackMontage() override;
	void ResetToFightAgain();
	void PlayArmDisarmMontage(const FName& SectionName);

	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	bool IfPlayerIsReadyToFightAgain();
	bool IfPlayerHasEquippedAWeapon();

	virtual bool CanAttack() override;

	UPROPERTY()
	class AProjectile* Projectile;

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void SpawnDefaultWeapon();
	void SpawnCrown();
	void ActivateCrown();

	UPROPERTY()
	TMap<FName, UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	bool bLeftGame = false;

	FOnPlayerLeavesGame PlayerLeavesGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);
	
	/****************** 
	** Moving Around **
	******************/ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;
	
	void Move(const FInputActionValue& Value);
	

	/*******************
	** Looking Around **
	*******************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;
	
	void Look(const FInputActionValue& Value);
	
	/**********************
	** Equipping Weapons **
	**********************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EquipAction;

	void EquipButtonPressed();
	void ToggleArmingAndDisarming();
	bool IfPlayerAlreadyEquippedAnyWeapon();

	/**************
	** Crouching **
	**************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;

	bool PlayerHasSword();
	void CrouchButtonPressed();

	/**********************
	** Aiming the Weapon **
	**********************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;

	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	bool SetTurningInPlaceEnum();
	void SimProxiesTurn();

	/************************
	****    Attacking    ****
	************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	/**********************
	****    Dodging    ****
	**********************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;


	void AttackButtonPressed();
	void AttackButtonReleased();

	void FireButtonPressed();
	bool PlayerNotUsingRangedOrMeleeWeapons();
	bool PlayerUsingMeleeWeapons();
	void FireButtonReleased();

	/*************************
	** Reloading the Weapon **
	*************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ReloadAction;

	void ReloadButtonPressed();
	bool PlayerNotUsingRangedWeapons();
	bool PlayerUsingRangedWeapons();

	/*************************
	** Throwing the Grenade **
	*************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrowAction;

	void GrenadeButtonPressed();
	void CalculateShieldDamage(float Damage, float& DamageToHealth);
	void DetermineRolesOnPlayerDeath(AActor* DamagedPawn, AController* InstigatorController);

	/*************************
	**  Swinging the Stick  **
	*************************/
	

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeaponBase* CharactersWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMeleeWeapon* CharactersMeleeWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ARangedWeapon* CharactersRangedWeapon;

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	bool CanDisarm();
	bool IfPlayerIsDisarmed();
	bool CanArm();
	bool bIsTogglingWeapon = false;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSpineSocket();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToMeleeSocket();

	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon Properties")
	void OnArmDisarmMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    virtual void MeleeAttack() override;
	bool IsOccupied();
	bool HasEnoughStamina();
	void Dodge();

	/*****************
	** Player Stats **
	*****************/

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float Shield = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float Majix = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxMajix = 100.f;
	
	FTimerHandle EliminationTimer;

	void EliminationTimerFinished();

	UPROPERTY(EditDefaultsOnly)
	float EliminationDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> HealingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> ShieldFortifyingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> StaminaRechargingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Effects")
	TSubclassOf<UGameplayEffect> MajixSummoningEffect;

	void Heal(float Magnitude);
	void Fortify(float Magnitude);
	void Recharge(float Magnitude);
	void Summon(float Magnitude);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* HAFMappingContext;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPlayerName();
	

	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	void RotateInPlace(float DeltaTime);

	/**
	* Hit boxes used for server-side rewind
	*/

	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

	void DropOrDestroyWeapon(AWeaponBase* Weapon);
	void DropOrDestroyBothWeapons();

	void SetSpawnPoint();
	void OnPlayerStateInitialized();


	
	UFUNCTION(BlueprintCallable)
	bool WeaponIsRanged();
	bool WeaponIsMelee();
	bool ItemIsPickup();
	void SetAllWeaponEnumsForRanged();
	void SetAllWeaponEnumsForMelee();
	void SetAllItemEnumsForPickup();
	void DisarmOneHandedWeapon(AMeleeWeapon* WeaponInHand);
	void DisarmTwoHandedWeapon(AMeleeWeapon* WeaponInHand);
	void ArmOneHandedWeapon(AMeleeWeapon* WeaponInHand);
	void ArmTwoHandedWeapon(AMeleeWeapon* WeaponInHand);

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingItem)
	class AItem* OverlappingItem;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeaponBase* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingItem(AItem* LastItem);

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastWeapon);

	UPROPERTY(VisibleInstanceOnly)
	class AMeleeWeapon* MeleeWeaponOverlapped;

	UPROPERTY(VisibleInstanceOnly)
	class ARangedWeapon* RangedWeaponOverlapped;

	UPROPERTY(VisibleInstanceOnly)
	AWeaponBase* WeaponOverlapped;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed(AWeaponBase* Weapon);

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

		
	/***********************
	** Animation Montages **
	***********************/

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* EliminatedMontage;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadingMontage;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;
	

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	UAnimMontage* ArmDisarmMontage;

	void HideCharacterIfCameraClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/********************
	** Dissolve Effect **
	********************/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//Dynamic Instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Eliminations)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//Material Instance set ono the blueprint used with the dynamic material instance
	UPROPERTY(VisibleAnywhere, Category = Eliminations)
	UMaterialInstance* DissolveMaterialInstance;

	/****************************
	****      TEAM COLORS    ****
	****************************/
	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* RedMaterial;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* RedDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* BlueDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* OriginalDissolveMaterialInstance;


	/****************************
	** Elimination-Bot/Effects **
	****************************/

	UPROPERTY(EditAnywhere)
	UParticleSystem* EliminationBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* EliminationBotComponent;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* EliminationBotSound;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* CrownComponent;

	/************
	** Grenade **
	************/
	UPROPERTY(VisibleAnywhere) 
	UStaticMeshComponent* AttachedGrenade;

	/*************************
	***   Default Weapon   ***
	*************************/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

public:
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void SetOverlappingWeapon(AWeaponBase* Weapon) override;
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AItem* GetItemThatOverlaps(AItem* ItemThatOverlaps);
	AWeaponBase* GetWeaponThatOverlaps(AWeaponBase* WeaponThatOverlaps);
	class AAmmoPickup* GetPickupThatOverlaps(class AAmmoPickup* PickupThatOverlaps);
	AWeaponBase* GetEquippedWeapon();
	FORCEINLINE FString GetRangedWeaponName() const { return Combat->EquippedRangedWeapon->RangedWeaponName; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bIsEliminated; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }	
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetMajix() const { return Majix; }
	FORCEINLINE float GetMaxMajix() const { return MaxMajix; }
	EActionState GetActionState() const;
	EWeaponState GetWeaponState() const;
	AHAFPlayerState* GetHAFPlayerState() const { return HAFPlayerState; }
	AFillainPlayerController* GetFillainPlayerController() const { return FillainPlayerController; }
	FORCEINLINE class AProjectile* GetProjectile() const { return Projectile; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadingMontage() const { return ReloadingMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return Buff; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE void SetStamina(float Amount) { Stamina = Amount; }
	FORCEINLINE void SetMajix(float Amount) { Majix = Amount; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	ETeam GetTeam();
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE AWeaponBase* GetOverlappingWeapon() const { return OverlappingWeapon; }
	FORCEINLINE AController* GetCachedEventInstigator() const { return CachedEventInstigator; }


};
