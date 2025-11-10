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
#include "BaseCharacter.generated.h"

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
class AHAFGameMode;
class UMotionWarpingComponent;
class UAnimMontage;
class AFillainCharacter;

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
	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	virtual void Dissolve();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	UFUNCTION(BlueprintCallable)
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UFUNCTION(BlueprintCallable, Category="HitReaction")
	FDirectionalHitResult DirectionalHitReact(const FVector& ImpactPoint);
	
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(AActor* DamagedPawn, float DamageAmount, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	virtual void PlayRandomMeleeAttackMontage();

	UFUNCTION(BlueprintCallable)
	virtual void PlayRandomMajixAttackMontage();
	
	virtual void PlayAttackMontage();
	
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
    
    virtual void CharacterDies();

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
	AController* CachedInstigatorController;

	UPROPERTY()
	AActor* CachedCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CoreComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AWeaponBase* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AMeleeWeapon* EquippedMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  ARangedWeapon* EquippedRangedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AFillainPlayerController* KillerPlayerController;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* AttributeComponent;

	float LastHitReactTime = 0.f;
	float HitReactCooldown = 0.4f; // seconds

	// Prevents hit react spam
	FTimerHandle HitReactTimer;
	bool bCanReact = true;

	void ResetHitReact();

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

	// --- GAS pointers (BaseCharacter only holds them; it doesn't create them) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Legacy-friendly getter used elsewhere in your project
	virtual UAttributeSet* GetAttributeSet() const;

	// Convenience (optional to keep your call sites tidy)
	UFUNCTION(BlueprintPure, Category="GAS|Attributes")
	virtual bool IsAbilityInStartupAbilities(TSubclassOf<UGameplayAbility> AbilityToCheck) const;

	void MaybeTriggerCharm(AActor* DamagedActor, AActor* DamageInstigator);

	void LogSecondaries_Client() const;
	void LogSecondaries_Server() const;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	
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

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	UPROPERTY(EditAnywhere, Category="Character")
	int32 CharacterLevel = 1;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo();
	void StopMontage(UAnimMontage* Montage);
	
	// If false, subclasses can opt out (critters/corpses/etc.)
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	bool bRequiresASC = true;

	// Call this instead of asserting in BeginPlay.
	void SafeInitASC_ForPawnOwner();          // For NPCs: ASC on Pawn
	void SafeInitASC_FromPlayerState();       // For players: ASC on PlayerState
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
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

	void AddCharacterAbilities() const;

	/*************************************
	*****                            *****
	*****     DISSOLVE MATERIALS     *****
	*****                            *****
	*************************************/
	

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance; 


private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

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