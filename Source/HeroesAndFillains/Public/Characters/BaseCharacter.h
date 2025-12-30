// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/CapsuleInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "CharacterClassInfo.h"
#include "Enemies/EnemyInfo.h"
#include "BaseCharacter.generated.h"

class UDebuffNiagaraComponent;
class AHAFGameMode;
class APCPickupBaseItem;
class APrePackagedPCPickupItem;
class UGameplayAbility;
struct FGameplayAttribute;
class UGameplayEffect;
class UHAFAttributeSet;
class UHAFAbilitySystemComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UMotionWarpingComponent;
class UAnimMontage;
class AFillainCharacter;
class AWeaponBase;
class AMeleeWeapon;
class ARangedWeapon;
class UNiagaraPassiveSpellComponent;

UENUM(BlueprintType, Blueprintable)
enum EDeathPose
{
	EDP_NotDead UMETA(DisplayName = "Not Dead"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	
	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FDirectionalHitResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bFromFront = false;

	UPROPERTY(BlueprintReadOnly)
	bool bFromBack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bFromLeft = false;

	UPROPERTY(BlueprintReadOnly)
	bool bFromRight = false;
};

UCLASS()
class HEROESANDFILLAINS_API ABaseCharacter : public ACharacter, public IHitInterface, public IAbilitySystemInterface, public ICombatInterface, public ICapsuleInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual UAnimMontage* GetHitReactionMontage_Implementation() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void MeleeAttack();
	virtual void MajixAttack();
	
	// ICombatInterface - BlueprintNativeEvent implementation
	virtual void Die(const FVector& DeathImpulse) override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	// Legacy damage system - kept as virtual stubs for FillainCharacter compatibility
	// TODO: Remove after FillainCharacter is migrated to pure GAS
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual void ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	virtual EEnemyType GetEnemyType_Implementation() override;
	UFUNCTION(BlueprintCallable)
	virtual void InitializeDefaultTags();

	/** Safely initializes attributes — waits until ASC is valid before calling InitializeDefaultAttributes */
	UFUNCTION()
	void SafeInitializeAttributes();

	UFUNCTION(BlueprintCallable)
	virtual void Dissolve();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	/** 
	 * Called by GAS when damage is applied through GameplayEffects
	 * This is the primary entry point for damage visualization (hit reactions, effects, etc.)
	 */
	UFUNCTION(BlueprintCallable)
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UFUNCTION(BlueprintCallable, Category="HitReaction")
	FDirectionalHitResult DirectionalHitReact(const FVector& ImpactPoint);

	virtual void PlayRandomMeleeAttackMontage();
	virtual void PlayRandomMajixAttackMontage();
	virtual USkeletalMeshComponent* GetSpellCaster_Implementation() override;
	UFUNCTION(BlueprintCallable)
    virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName CombatSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	double Theta = 5.123221212212;
	
    virtual bool CanAttack();
    
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	
	/*****************************
    ***                        ***
    ***   ANIMATION MONTAGES   ***
    ***                        ***
    *****************************/

    UPROPERTY(EditDefaultsOnly, Category = Montages)
    class UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, Category = Montages)
    class UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactionMontage;
	
	UPROPERTY()
	TArray<FGameplayTag> AttackTags;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* MeleeAttackMontage;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> MeleeAttackMontages;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> MeleeAttackMontageSections;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* MajixAttackMontage;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> MajixAttackMontages;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> MajixAttackMontageSections;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Setup")
	UCharacterClassInfo* CharacterClassInfo;
	
	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	UFUNCTION(BlueprintCallable)
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	
	UFUNCTION(BlueprintCallable)
	virtual int32 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> SectionNames);

	virtual void PlayAttackMontage(const FGameplayTag& InputTag);
	virtual void PlayHitReactMontage(const FName& SectionName);
    virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopAllMontages(float BlendOutTime = 0.25f);
	
    UPROPERTY(EditAnywhere, Category = "Sound")
    class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* HitReactParticles;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	class UNiagaraSystem* HitReactSystem;

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitSpecialEffects(const FVector& ImpactPoint);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose = EDeathPose::EDP_NotDead;

	void DisableCapsule();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CoreComponent;
	
	// Primary weapon reference
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeaponBase* EquippedWeapon;

	// Typed weapon references for convenience
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	AMeleeWeapon* EquippedMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	ARangedWeapon* EquippedRangedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AFillainPlayerController* KillerPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AController* CachedInstigatorController;

	// Legacy AttributeComponent - TODO: Remove after migrating all code to GAS
	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* AttributeComponent;

	// Legacy cached damage parameters - TODO: Remove after migrating to GAS-only damage
	UPROPERTY()
	float CachedDamageAmount = 0.f;

	UPROPERTY()
	FDamageEvent CachedDamageEvent;

	UPROPERTY()
	AController* CachedEventInstigator = nullptr;

	UPROPERTY()
	AActor* CachedDamageCauser = nullptr;

	UPROPERTY()
	AActor* CachedDamagedPawn;

	UPROPERTY()
	float CachedDamage;

	UPROPERTY()
	const UDamageType* CachedDamageType;

	UPROPERTY()
	AActor* CachedCauser;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Warping")
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY()
	AHAFGameMode* HAFGameMode;

	bool bIsEliminated = false;

	void DisableMeshCollision();

	// --- GAS Components ---
	// Note: BaseCharacter holds these pointers but doesn't create them
	// FillainCharacter gets ASC from PlayerState, EnemyBase creates its own
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Legacy-friendly getter used elsewhere in your project
	virtual UAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintPure, Category="GAS|Attributes")
	virtual bool IsAbilityInStartupAbilities(TSubclassOf<UGameplayAbility> AbilityToCheck) const;

	void LogSecondaries_Client() const;
	void LogSecondaries_Server() const;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName = FName("LeftHandSocket");

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName = FName("RightHandSocket");

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftFootSocketName = FName("LeftFootSocket");

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightFootSocketName = FName("RightFootSocket");

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TeethSocketName = FName("TeethSocket");
	
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool WasBaseCharacterHit = false;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool IsAlive() const;
	
	UFUNCTION(BlueprintPure, Category="Character|Attributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool IsDying() const;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool IsCharacterAlive() const;

	UFUNCTION(BlueprintCallable, Category="GAS|Costs")
	virtual void ConsumeDodgeStamina();

	UPROPERTY(EditDefaultsOnly, Category="GAS|Costs")
	TSubclassOf<class UGameplayEffect> GE_DodgeStaminaCost;

	UPROPERTY(EditDefaultsOnly, Category="GAS|Costs")
	float DodgeStaminaCost = 14.f;

	UFUNCTION(BlueprintPure, Category="Costs")
	float GetDodgeCost() const { return DodgeStaminaCost; }

	virtual TArray<FVector> GetCombatSocketLocations_Implementation(const FGameplayTag& SocketTag) override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	UPROPERTY(EditAnywhere, Category="Character")
	int32 CharacterLevel = 1;
	
	int32 MinionCount = 0;

	virtual int32 GetMinionCount_Implementation() override;
    virtual void IncrementMinionCount_Implementation(int32 Amount) override;

	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override; 
	
	FOnASCRegistered OnASCRegistered;
	FOnDeathSignature OnDeathDelegate;
	FOnDamageSignature OnDamageDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;

	UPROPERTY(ReplicatedUsing = OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing = OnRep_BeingShocked, BlueprintReadOnly)
	bool bIsBeingShocked = false;
	
	UFUNCTION()
	virtual void OnRep_Burned();
	
	UFUNCTION()
	virtual void OnRep_Stunned();

	UFUNCTION()
	virtual void OnRep_BeingShocked();

	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual FOnDamageSignature& GetOnDamageDelegate() override ;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintCallable)
	virtual void UnlockAbilities();

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<UGameplayAbility>> GetStartupAbilities();

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<UNiagaraPassiveSpellComponent> HaloOfProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<UNiagaraPassiveSpellComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<UNiagaraPassiveSpellComponent> MightOfMajixNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<UNiagaraPassiveSpellComponent> DoubleJeopardyNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<UNiagaraPassiveSpellComponent> FlightOfTheFeeniksNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PassiveAbilities")
	TObjectPtr<USceneComponent> EffectAttachComponent;
protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo();
	void StopMontage(UAnimMontage* Montage);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Type Defaults")
	EEnemyType EnemyType = EEnemyType::None;
	
	// If false, subclasses can opt out (critters/corpses/etc.)
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	bool bRequiresASC = true;

	// ASC initialization helpers
	void SafeInitASC_ForPawnOwner();          // For NPCs: ASC on Pawn
	void SafeInitASC_FromPlayerState();       // For players: ASC on PlayerState
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* CurrentAttackMontage;

	bool bIsCharacterDead{false};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultResistanceAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultInvisibleAttributes;

	UFUNCTION(BlueprintCallable)
	virtual void ApplyStartupEffects() const;

	UFUNCTION(BlueprintCallable)
	virtual void InitializeDefaultAttributes();

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level);

	UFUNCTION(BlueprintCallable)
	virtual void AddCharacterAbilities(); 

	
	/*************************************
	*****                            *****
	*****     DISSOLVE MATERIALS     *****
	*****                            *****
	*************************************/

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineZero(UMaterialInstanceDynamic* CharacterDynamicMaterialInstanceZero);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimelineZero(UMaterialInstanceDynamic* WeaponDynamicMaterialInstanceZero);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInstanceZero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstanceZero; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* BloodEffect;

	
	
private:
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<UGameplayAbility>> GetStartupPassiveAbilities();
public:
	FORCEINLINE AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE AMeleeWeapon* GetEquippedMeleeWeapon() const { return EquippedMeleeWeapon; }
	FORCEINLINE ARangedWeapon* GetEquippedRangedWeapon() const { return EquippedRangedWeapon; }
	FORCEINLINE int32 GetCharacterLevel() const { return CharacterLevel; }
	FORCEINLINE void SetCharacterLevel(const int32 NewLevel) { CharacterLevel = FMath::Clamp(NewLevel, 1, 100); }
	
	static float SafeGetNumeric(const UAbilitySystemComponent* ASC,
							const UHAFAttributeSet* AS,
							const FGameplayAttribute& Attr);
	static float SafeGet(const UAbilitySystemComponent* ASC, const UHAFAttributeSet* AS, const FGameplayAttribute& Attr);
};
