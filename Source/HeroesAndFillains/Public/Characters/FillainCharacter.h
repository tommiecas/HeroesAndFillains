// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "UI/OverheadWidget.h"
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
#include "Interfaces/CapsuleInterface.h"
#include "Interfaces/PlayerInterface.h"
#include "AbilitySystemComponent.h" // <-- for FOnAttributeChangeData
#include "GameplayEffectTypes.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/CharacterTypes.h"
#include "FillainCharacter.generated.h"

class AHAFMajixProjectile;
class AMajixWeapon;
class UGameplayEffect;
class AWeaponBase;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FInputActionInstance;
struct FOnAttributeChangeData;
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
class UHAFAttributeSet;



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLeavesGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageWidget, float, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvisibleAttributeChangedSignature, float, NewValue);

UCLASS()
class HEROESANDFILLAINS_API AFillainCharacter : public ABaseCharacter, public IInteractWithCrosshairsInterface, public IPickupInterface, public IPlayerInterface 
{
	GENERATED_BODY()

public:
	AFillainCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void InitializeDefaultTags() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void ApplyStartupEffects() const override;
	bool bIsDead = false;

	virtual void AddToXP_Implementation(int32 XPToAdd) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 XP) override;
	virtual int32 GetAttributePointsAward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsAward_Implementation(int32 Level) const override;
	virtual void AddToCharacterLevel_Implementation(int32 LevelToAdd) override;
	virtual void AddToAttributePoints_Implementation(int32 AttributePointsToAdd) override;
	virtual void AddToSpellPoints_Implementation(int32 SpellPointsToAdd) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	
	// Override legacy damage system (TODO: Remove after migrating to pure GAS)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void InitializeDefaultAttributes() override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void AddSoulsGatheredToTotalSouls(class ASoul* Soul) override;
	virtual void AddGoldAcquiredToTotalGold(class ATreasure* Treasure) override;
	virtual void OnRep_Burned() override;
	virtual void OnRep_Stunned() override;
	void InitASC();

	UFUNCTION(BlueprintCallable)
	int32 PlayProjectileSpellMontage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	UAnimMontage* ProjectileSpellMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	TArray<FName> ProjectileSpellMontageSections;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_OverlappingItem)
	APCPickupBaseItem* OverlappingItem;

	UFUNCTION()
	virtual void OnRep_OverlappingItem(APCPickupBaseItem* LastItem);

	UFUNCTION()
	virtual void OnRep_OverlappingWeapon(AWeaponBase* LastWeapon);

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeaponBase* OverlappingWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingMeleeWeapon)
	AMeleeWeapon* OverlappingMeleeWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingRangedWeapon)
	ARangedWeapon* OverlappingRangedWeapon;

	UFUNCTION()
	virtual void OnRep_OverlappingMeleeWeapon(AMeleeWeapon* LastMeleeWeapon);

	UFUNCTION()
	virtual void OnRep_OverlappingRangedWeapon(ARangedWeapon* LastRangedWeapon); 

	UFUNCTION(BlueprintCallable)
	void ApplyRegenerationEffects();
	
	
	virtual void PlayAttackMontage(const FGameplayTag& InputTag) override;
	void PlayRandomMeleeAttackMontage();
	void PlayRandomMajixAttackMontage();
	
	// Prevent re-entrancy / double-trigger

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(class AWeaponBase* WeaponToEquip);

	
	/*******************************
	****    Combat Interface    ****
	*******************************/

	virtual int32 GetCharacterLevel_Implementation(ABaseCharacter* Character) override;

	/********************************
	****    Capsule Interface    ****
	********************************/

	virtual double GetCharacterCapsuleHeight() override;
	virtual double GetCharacterCapsuleRadius() override;
	virtual void SetCharacterCapsuleHeight(double Height) override;
	virtual void SetCharacterCapsuleRadius(double Radius) override;
	
	// virtual void Restart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EquipOneHandedRangedWeapon(AWeaponBase* W);
	void EquipTwoHandedRangedWeapon(AWeaponBase* Wpn);
	void EquipOneHandedMeleeWeapon(AWeaponBase* Wn);
	void EquipTwoHandedMeleeWeapon(AWeaponBase* WeaponB);
	void EquipOneHandedMajixWeapon(AWeaponBase* MajixWeapon);
	void EquipTwoHandedMajixWeapon(AWeaponBase* THMWeapon);
	void EquipWeapon(AWeaponBase* AWB);
	void ActivateCombatCharacter();
	void InitializeBuffProperties();
	void ConfigureLagCompensation();
	virtual void InitializeAbilityActorInfo() override;
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
	virtual void Destroyed() override;
	void HideAttachedGrenade();
	// void OnFillainDying(AFillainCharacter* InstigatorFillain, AFillainCharacter* DyingFillain, class AFillainPlayerController* InstigatorController);
	void UpdateHUDAmmo();
	void SwitchWeapon(AWeaponBase* NewWeapon);
	ARangedWeapon* EquippedWeaponIsARangedWeapon();
	AMeleeWeapon* EquippedWeaponIsAMeleeWeapon();
	AMajixWeapon* EquippedWeaponIsAMajixWeapon();


	bool WeaponIsUnclaimedFirearm(ARangedWeapon* Ranged);
	bool WeaponIsUnclaimedMeleeWeapon(AMeleeWeapon* Melee);

	UFUNCTION(BlueprintCallable)
	bool IsUsingGamepad() const;


	UPROPERTY(VisibleAnywhere, Category = Combat)
	EBattlePrepped BattlePrepped = EBattlePrepped::EBP_Defenseless;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AMeleeWeapon* AcquiredMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AFillainHUD* FillainHUD;
	
	UPROPERTY()
	AFillainPlayerController* FillainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCombatComponent* CombatComponent;

	UPROPERTY()
	class AHAFPlayerState* HAFPlayerState;

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


	UPROPERTY(BlueprintAssignable, Category="UI|Events")
	FOnMessageWidget OnMessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ShiftAction;
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
	void ResetToFightAgain();
	void PlayArmDisarmMontage(const FName& SectionName);

	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	bool IfPlayerHasEquippedAWeapon();

	virtual bool CanAttack() override;

	UPROPERTY()
	class AProjectile* Projectile;


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
	void ToggleArmingAndDisarming(AWeaponBase* EquippedWeapon);
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
	void DetermineRolesOnPlayerDeath(AActor* DamagedPawn, AController* InstigatorController);

	/*************************
	**  Swinging the Stick  **
	*************************/

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMeleeWeapon* CharactersMeleeWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ARangedWeapon* CharactersRangedWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMajixWeapon* CharactersMajixWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeaponBase* CharactersWeapon;

	UPROPERTY()
	bool bIsTogglingWeapon = false;

	UPROPERTY()
	bool bEquipInProgress = false;

	UPROPERTY(EditAnywhere, Category = Eliminations)
	UMaterialInstance* DissolveMaterialInstance;

	UFUNCTION(Client, Reliable)
	void Client_OnEquipped();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	bool CanDisarm();
	bool IfPlayerIsDisarmed();
	bool CanArm();

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
	virtual void MajixAttack() override;
	bool IsOccupied();
	bool HasEnoughStamina(const float Cost) const;
	void Dodge();
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
	UHAFAttributeSet* HAFAttributes;

	UPROPERTY() bool bASCReady = false;

	/***************************************
	****	  INTUITION-BASED           ****
	****    "CAPSULE" ADJUSTMENT        ****
	***************************************/

	UPROPERTY(Transient)
	bool bAttrHooksBound = false;
	
	/***************************************
	****    ENEMY CAPSULE ADJUSTMENT    ****
	***************************************/

	float GetHitAssistPaddingCM();

	UFUNCTION(BlueprintCallable)
	void FireAtCursor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHAFMajixProjectile> HAFProjectileClass;

	UFUNCTION(BlueprintCallable)
	 void SetOverlaps(APCPickupBaseItem* FloatingItem);

	UPROPERTY(EditDefaultsOnly, Category="GAS|Damage")
	TSubclassOf<class UGameplayEffect> GE_DamageSplit;

#pragma region Combat State

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CombatRadius = 2200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CombatTimeout = 7.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float LastCombatTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsInCombat = false;

	// Functions
	void EnterCombat();
	void ExitCombat();
	void UpdateCombatState(float DeltaTime);
	bool AreEnemiesNearby(float Radius);

#pragma endregion
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float HitAssistPaddingCM = 0.f;
	
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
	void DisarmOneHandedWeapon(AWeaponBase* WeaponInHand);
	void DisarmTwoHandedWeapon(AWeaponBase* WeaponInHand);
	void ArmOneHandedWeapon(AWeaponBase* WeaponInHand);
	void ArmTwoHandedWeapon(AWeaponBase* WeaponInHand);

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void ApplyDefaultAttributes();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	
private:
	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	bool bSelfOccluded = false;            // tracks current hide state

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraSelfOcclusionThreshold = 160.f; // tweakable

	void ResetCameraRig();

	UFUNCTION()
	void HideCharacterIfCameraClose();
	
	/******************************************
	****    TREASURE CAPSULE ADJUSTMENT    ****
	******************************************/
	
	UFUNCTION()
	void BindHiddenTreasureCapsuleHooksOnce();

	/*****************************************
	****    FILLAIN CAPSULE ADJUSTMENT    ****
	*****************************************/
	
	UFUNCTION()
	void BindFillainCharacterCapsuleHooksOnce();

	void OnFillainCharacterCapsuleScaleDriverChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(Server, Reliable)
	void Server_ApplyFillainCharacterCapsuleFromStats();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyFillainCharacterCapsuleSize(float TargetUnscaledHalf, float TargetUnscaledRadius);

	bool bFillainCharacterCapsuleHooksBound = false;
	bool bDidInitialFillainCharacterCapsuleApply = false;
	bool bFillainCharacterCapsuleInitialized = false;
	bool bFillainCharacterCapsuleBaselinesInit = false;
	bool InitFillainCharacterCapsuleBaselinesIfNeeded();

	// Exact signature the delegate expects:
	void OnIntuitionChanged    (const FOnAttributeChangeData& Data);
	void OnVisionChanged(const FOnAttributeChangeData& Data);
	
	// GAS
	UPROPERTY() class UAbilitySystemComponent* ASC = nullptr;

	// Baselines (set in BeginPlay)
	float StandingUnscaledHalfHeight = 0.f;
	float StandingUnscaledRadius     = 0.f;
	float StandingScaledHalfHeight   = 0.f;
	float StandingMeshRelZ           = 0.f;

	// Optional mesh correction if your skeleton root isn't at the soles
	UPROPERTY(EditAnywhere, Category="Capsule")
	float FeetToRootZOffset = 0.f;

	// === Update coalescing (so Agi/Flex/Dex firing together calls us once) ===
	FTimerHandle FillainCharacterCapsuleUpdateTimer;
	void RequestFillainCharacterCapsuleUpdate();             // queue for next tick
	void ApplyFillainCharacterCapsuleFromCurrentStats();     // reads attributes and calls ApplyCapsuleSize_FeetPlanted

	// (Optional) tiny change filter so we don't spam tiny resizes
	float LastAppliedHalf   = -1.f;
	float LastAppliedRadius = -1.f;
	
	// You already have this from earlier:
	void ApplyFillainCharacterCapsuleSize_FeetPlanted(float TargetUnscaledHalf, float TargetUnscaledRadius);

	void RestoreStandingFillainCharacterCapsule();

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Camera") float DefaultFOV = 90.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera") float MinFOV = 60.f;   // safe floor
	UPROPERTY(EditDefaultsOnly, Category="Camera") float MaxFOV = 110.f;  // optional
	bool bFOVLock = false;
	float FOVLockTimeLeft = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	
	UPROPERTY(VisibleInstanceOnly)
	class AMeleeWeapon* MeleeWeaponOverlapped;

	UPROPERTY(VisibleInstanceOnly)
	class ARangedWeapon* RangedWeaponOverlapped;

	UPROPERTY(VisibleInstanceOnly)
	AWeaponBase* WeaponOverlapped;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed(AWeaponBase* Weap);



	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float SelfOcclEnter = 160.f;   // enter hide below this

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float SelfOcclExit  = 240.f;   // leave hide above this

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float SelfOcclMinHold = 0.25f; // seconds to hold state before allowing a switch

	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	float SelfOcclStateTime = 0.f; // internal timer

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
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
	UFUNCTION(BlueprintCallable)
	virtual void SetOverlappingItem(APCPickupBaseItem* HoveringItem) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetOverlappingWeapon(AWeaponBase* HoveringWeapon) override;
	
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE APCPickupBaseItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE AWeaponBase* GetOverlappingWeapon() const { return OverlappingWeapon; }
	FORCEINLINE AMeleeWeapon* GetOverlappingMeleeWeapon() const { return OverlappingMeleeWeapon; }
	FORCEINLINE ARangedWeapon* GetOverlappingRangedWeapon() const { return OverlappingRangedWeapon; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	APrePackagedPCPickupItem* GetItemThatOverlaps(APrePackagedPCPickupItem* ItemThatOverlaps);
	AWeaponBase* GetWeaponThatOverlaps(AWeaponBase* WeaponThatOverlaps);
	class AAmmoPickup* GetPickupThatOverlaps(class AAmmoPickup* PickupThatOverlaps);
	AWeaponBase* GetEquippedWeapon();
	FORCEINLINE FString GetRangedWeaponName() const { return CombatComponent->EquippedRangedWeapon->RangedWeaponName; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bIsEliminated; }
	/*FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }	
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetMajix() const { return Majix; }
	FORCEINLINE float GetMaxMajix() const { return MaxMajix; }*/
	EActionState GetActionState() const;
	EWeaponState GetWeaponState() const;
	AHAFPlayerState* GetHAFPlayerState() const { return HAFPlayerState; }
	AFillainPlayerController* GetFillainPlayerController() const { return FillainPlayerController; }
	FORCEINLINE class AProjectile* GetProjectile() const { return Projectile; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadingMontage() const { return ReloadingMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	ETeam GetTeam();
	FORCEINLINE void SetHealth(const float Amount) const { if (HAFAttributes) HAFAttributes->SetHealth(Amount); }
	FORCEINLINE void SetShield(const float Amount) const { if (HAFAttributes) HAFAttributes->SetShield(Amount); }
	FORCEINLINE void SetStamina(const float Amount) const { if (HAFAttributes) HAFAttributes->SetStamina(Amount); }
	FORCEINLINE void SetMajix(const float Amount) const { if (HAFAttributes) HAFAttributes->SetMajix(Amount); }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE EBattlePrepped GetBattlePrepped() const { return BattlePrepped; }
	FORCEINLINE AFillainHUD* GetHUD() const { return FillainHUD; }
};




