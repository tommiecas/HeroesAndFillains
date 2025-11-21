// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionWarping.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterClassInfo.h"
#include "Delegates/AttributeDelegates.h" // ✅ Must come before generated.h
#include "Interfaces/EnemyAttributeMenuWidgetControllerInterface.h"
#include "Interfaces/EnemyInterface.h"
#include "Interfaces/HitInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyBase.generated.h"

class UCombatComponent;
enum class ECharacterClass : uint8;
class UTimelineComponent;
class UBoxComponent;
class UEnemyWidgetControllerBase;
class UWidgetComponent;
class UHAFAbilitySystemComponent;
class UHAFAttributeSet;
class UEnemyStatsWidget;
class UEnemyAttributeMenuWidget;
class AAIController;
class ASoul;
class AWeaponBase;
class AMeleeWeapon;
class ARangedWeapon;
class UEnemyProgressBarBaseWidget;
class UAttributeInfo;
class AHAFAIController;
class UBehaviorTreeComponent;
class UBehaviorTree;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Patrolling      UMETA(DisplayName = "Patrolling"),
    EES_Chasing         UMETA(DisplayName = "Chasing"),
    EES_Attacking       UMETA(DisplayName = "Attacking"),
    EES_Engaged         UMETA(DisplayName = "Engaged"),
    EES_Dead            UMETA(DisplayName = "Dead"),
    EES_Idle            UMETA(DisplayName = "Idle")
};

UCLASS()
class HEROESANDFILLAINS_API AEnemyBase  : public ABaseCharacter, public IEnemyInterface, public IEnemyAttributeMenuWidgetControllerInterface
{
    GENERATED_BODY()

public:
    AEnemyBase();

    virtual void BeginPlay() override;
    void DeferredGASSetup();

    virtual void Tick(float DeltaTime) override;
    virtual void InitializeDefaultTags() override;
    virtual void InitializeDefaultAttributes() override;
    virtual void PossessedBy(AController* NewController) override;
    void SafeInitializeAttributes();
    virtual void OnRep_PlayerState() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // --- Interface ---
    virtual UAbilitySystemComponent* GetEnemyASC_Implementation() const override;
    virtual UAttributeSet* GetEnemyAttributeSet_Implementation() const override;
    virtual void BroadcastEnemyAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
    virtual void BindCallbacksToDependencies_Implementation() override;
    virtual void BroadcastInitialEnemyValues_Implementation() override;

    // --- Combat / AI ---
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator, AActor* DamageCauser) override;
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    virtual void Die_Implementation() override;
    virtual void Dissolve() override;
    virtual void Destroyed() override;
    virtual int32 PlayDeathMontage() override;

    UFUNCTION(BlueprintCallable)
    void Attack();
    
    virtual void MeleeAttack() override;
    virtual void MajixAttack() override;

    UFUNCTION(BlueprintCallable)
    void RangedAttack();
    
    // --- Ranged Weapon Firing System ---
    
    /** Check if enemy can fire weapon (all conditions met) */
    bool CanFireWeapon() const;
    
    /** Check if enemy has clear line of sight to target */
    bool HasLineOfSight(AActor* Target) const;
    
    /** Calculate aim target with accuracy variation */
    FVector CalculateAimTarget() const;
    
    /** Fire the equipped ranged weapon at target */
    void FireWeapon();
    
    /** Reset fire cooldown timer */
    void ResetFireCooldown();
    
    virtual void PlayRandomMeleeAttackMontage() override;
    virtual void PlayRandomMajixAttackMontage() override;
    virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    virtual bool CanAttack() override;
    virtual void AttackEnd() override;

    void HideEnemyStatWidgets();
    void ShowEnemyStatWidgets();


    // --- Delegates ---
    UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
    FAttributeInfoSignature EnemyAttributeInfoDelegate;
    
    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyMaxHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyShieldChanged; // [Fixed] Typo corrected

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyMaxShieldChanged; // [Fixed] Typo corrected

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyMaxStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyMajixChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnAttributeChanged OnEnemyMaxMajixChanged;

    void HandleChangeInHealth(const FOnAttributeChangeData& Data);
    void HandleChangeInMaxHealth(const FOnAttributeChangeData& Data);
    void HandleChangeInShield(const FOnAttributeChangeData& Data);
    void HandleChangeInMaxShield(const FOnAttributeChangeData& Data);
    void HandleChangeInStamina(const FOnAttributeChangeData& Data);
    void HandleChangeInMaxStamina(const FOnAttributeChangeData& Data);
    void HandleChangeInMajix(const FOnAttributeChangeData& Data);
    void HandleChangeInMaxMajix(const FOnAttributeChangeData& Data);

    UPROPERTY()
    UEnemyWidgetControllerBase* EnemyWidgetController;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEnemyWidgetControllerBase> EnemyWidgetControllerClass;

    // Pointer to your health widget controller
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
    UEnemyWidgetControllerBase* EnemyHealthWidgetController;
    
    // Pointer to your shield widget controller
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
    UEnemyWidgetControllerBase* EnemyShieldWidgetController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UEnemyWidgetControllerBase> EnemyHealthWidgetControllerClass;
    
    // Pointer to your shield widget controller
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UEnemyWidgetControllerBase> EnemyShieldWidgetControllerClass;
    
    // --- AI Logic ---
    void InitializeEnemy();

    UFUNCTION(BlueprintCallable)
    AHAFAIController* LaunchEnemyHAFAIController();

    UFUNCTION(BlueprintCallable)
    virtual void RegisterAttackCollision(UBoxComponent* CollisionBox);

    void CheckPatrolTarget();
    void CheckCombatTarget();
    void PatrolTimerFinished();
    void EnemiesLoseInterest();
    void EnemiesStartPatrolling();
    void EnemiesChaseTarget();
    bool IsOutsideCombatRadius();
    bool IsOutsideAttackRadius();
    bool IsInsideAttackRadius();
    bool IsEnemyChasing() const;
    bool IsEnemyAttacking() const;
    bool IsEnemyEngaged() const;
    bool IsEnemyDead() const ;
    void ClearPatrolTimer();
    void StartAttackTimer();
    void ClearAttackTimer();
    bool InTargetRange(AActor* Target, double Radius) const;
    void MoveToTarget(AActor* Target) const;
    AActor* ChoosePatrolTarget();
    void PawnSeen(APawn* SeenPawn);

    UPROPERTY(EditAnywhere, Category = Combat)
    float PatrollingSpeed = 125.f;

    UPROPERTY(EditAnywhere, Category = Combat)
    float ChasingSpeed = 300.f;
	
    UPROPERTY(EditAnywhere, Category = Combat)
    float DeathLifeSpan = 8.f;
    
    // --- Ranged Combat Properties ---
    
    /** Time between shots (reload time) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Ranged")
    float FireCooldownTime = 3.0f;
    
    /** Weapon accuracy (0.0 = always miss, 1.0 = perfect accuracy) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Ranged", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float WeaponAccuracy = 0.7f;
    
    /** Maximum range for firing weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Ranged")
    float MaxFiringRange = 2000.0f;
    
    /** Can the enemy fire right now (cooldown control) */
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Ranged")
    bool bCanFire = true;
    
    /** Timer handle for fire cooldown */
    FTimerHandle FireCooldownTimer;
    
    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // --- Weapons ---
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SpawnEnemyWeapon();
    
    // You don't need to manually declare _Implementation in the header
    // Unreal's code generation will handle this

    // --- UI / Widgets ---
    void InitializeEnemyWidgets();
    void InitializeEnemyAttributeMenu();
    void UpdateEnemyAttributeMenu();

    UFUNCTION(BlueprintCallable, Category = "Visual")
    virtual void HighlightActor() override;

    UFUNCTION(BlueprintCallable, Category = "Visual")
    virtual void UnHighlightActor() override;

    UFUNCTION(BlueprintCallable, Category = "Visual")
    virtual void OnHoverStart() override;

    UFUNCTION(BlueprintCallable, Category = "Visual")
    virtual void OnHoverEnd() override;

    // --- GAS ---
    virtual void InitializeAbilityActorInfo() override;
    void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                      class AController* EventInstigator, AActor* DamageCauser);

    // --- Soul ---
    void SpawnSoul();
    FGenericTeamId GetGenericTeamId() const;

    // --- Helpers ---
    virtual int32 GetPlayerLevel() override;
    virtual void MulticastHandleDeath_Implementation() override;
    void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

    void SetEnemyState(EEnemyState NewState);          // [Restored]
    bool IsAttacking() const;                          // [Restored]
    bool IsPatrolling() const;                         // [Restored]
    bool bHitReacting = false;

    virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
    virtual AActor* GetCombatTarget_Implementation() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float BaseWalkSpeed = 250.f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    EEnemyState EnemyState = EEnemyState::EES_Idle;
    
    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GAS")
    UHAFAbilitySystemComponent* EnemyAbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GAS")
    UHAFAttributeSet* EnemyAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    UAttributeInfo* EnemyAttributeInfoOverride; // [Restored]

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
    UAISenseConfig_Sight* SightConfig;

    // --- Widgets ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* HealthBarWidgetComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* ShieldBarWidgetComponent;

    UPROPERTY()
    UEnemyAttributeMenuWidget* EnemyAttributeMenuWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UEnemyAttributeMenuWidget> EnemyAttributeMenuWidgetClass;

    // --- Weapons ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    TArray<UBoxComponent*> AttackCollisions;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    AWeaponBase* EquippedEnemyWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    AMeleeWeapon* EquippedEnemyMeleeWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    ARangedWeapon* EquippedEnemyRangedWeapon;

    // --- AI ---
    UPROPERTY()
    AHAFAIController* EnemyController;

    // --- Gameplay Data ---
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float PatrolRadius = 400.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float AttackRadius = 150.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float AcceptanceRadius = 150.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float CombatRadius = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float PatrolWaitMin = 3.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float PatrolWaitMax = 7.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float AttackMin = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float AttackMax = 2.f;

    FTimerHandle PatrolTimer;
    FTimerHandle AttackTimer;

    UPROPERTY()
    UEnemyProgressBarBaseWidget* HealthBarWidget;

    UPROPERTY()
    UEnemyProgressBarBaseWidget* ShieldBarWidget;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEnemyProgressBarBaseWidget> HealthBarWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEnemyProgressBarBaseWidget> ShieldBarWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Souls")
    TSubclassOf<ASoul> SoulClass;

    FGenericTeamId TeamId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FText EnemyDisplayName;
    
    virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) override;
    
    virtual void ResetCanDamage();
    
    
    // GAS Interface implementations
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UAbilitySystemComponent* GetEnemyASC() const; 

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UAttributeSet* GetEnemyAttributeSet() const;

    UPROPERTY()
    TArray<AActor*> DamagedActors;

    UFUNCTION(BlueprintCallable, Category = "Motion Warping")
    void SetWarpTargetsForCombatTarget(AActor* TargetActor);
    
    // Virtual, empty by default — so each enemy can override
    UFUNCTION(BlueprintCallable, Category="Combat|Collision")
    virtual void EnableLeftSideMeleeAttack() {}

    UFUNCTION(BlueprintCallable, Category="Combat|Collision")
    virtual void DisableLeftSideMeleeAttack() {}

    UFUNCTION(BlueprintCallable, Category="Combat|Collision")
    virtual void EnableRightSideMeleeAttack() {}

    UFUNCTION(BlueprintCallable, Category="Combat|Collision")
    virtual void DisableRightSideMeleeAttack() {}

    // Current patrol target
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    AActor* PatrolTarget;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<AActor*> PatrolTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
    TSubclassOf<class AWeaponBase> BaseWeaponClass;

    UPROPERTY(EditAnywhere, Category = Combat)
    TSubclassOf<UCombatComponent> CombatComponentClass;

    // Add this override
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LifeSpan = 5.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY()
    TObjectPtr<AHAFAIController>HAFAIController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
    ECharacterClass CharacterClass = ECharacterClass::Warrior;

    UFUNCTION()
    virtual void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    void SafeInitASC_ForPawnOwner();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BindCallbacksToDependencies();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BroadcastInitialEnemyValues();

    UPROPERTY(BlueprintReadWrite)
    bool bDead = false;

   

    UPROPERTY()
    bool bCanDamage = true;

    FTimerHandle DamageResetTimer;

    // Base damage for melee attacks
    UPROPERTY(EditAnywhere, Category = "Combat")
    float BaseDamage = 20.f;

    // Death effects
    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* DeathParticles;

    UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* DeathSound;

    // Soul spawning
    UPROPERTY(EditAnywhere, Category = "Soul")
    int32 SoulValue = 10;

    // Dissolve effect
    UPROPERTY()
    UTimelineComponent* DissolveTimeline;

    // GAS damage effect class
    UPROPERTY(EditAnywhere, Category = "Combat|Damage")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    // Hover state for UI
    UPROPERTY()
    bool bIsHovered = false;

    // Hit effects
    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* HitParticles;

    // Currently active hover menu widget instance
    UPROPERTY()
    UEnemyAttributeMenuWidget* ActiveAttributeMenuWidget = nullptr;

   
public:
    FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }
    FORCEINLINE FText GetEnemyDisplayName() const { return EnemyDisplayName; }
    FORCEINLINE UEnemyProgressBarBaseWidget* GetHealthBarWidget() const { return HealthBarWidget; }
    FORCEINLINE UEnemyProgressBarBaseWidget* GetShieldBarWidget() const { return ShieldBarWidget; }
};
