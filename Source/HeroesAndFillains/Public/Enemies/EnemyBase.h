// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionWarping.h"
#include "Characters/BaseCharacter.h"
#include "Delegates/AttributeDelegates.h" // ✅ Must come before generated.h
#include "Interfaces/EnemyAttributeMenuWidgetControllerInterface.h"
#include "Interfaces/EnemyInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyBase.generated.h"

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
    virtual void Tick(float DeltaTime) override;
    virtual void PossessedBy(AController* NewController) override;
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
    virtual void Die() override;
    virtual void Dissolve() override;
    virtual int32 PlayDeathMontage() override;

    virtual void MeleeAttack() override;
    virtual void MajixAttack() override;
    virtual void PlayAttackMontage() override;
    virtual void PlayRandomMeleeAttackMontage() override;
    virtual void PlayRandomMajixAttackMontage() override;
    virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    virtual bool CanAttack() override;
    virtual void AttackEnd() override;
    virtual void InitializeDefaultAttributes() const override;

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
    bool IsEnemyChasing();
    bool IsEnemyAttacking();
    bool IsEnemyEngaged();
    bool IsEnemyDead();
    void ClearPatrolTimer();
    void StartAttackTimer();
    void ClearAttackTimer();
    bool InTargetRange(AActor* Target, double Radius);
    void MoveToTarget(AActor* Target);
    AActor* ChoosePatrolTarget();
    void PawnSeen(APawn* SeenPawn);

    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

    // --- Weapons ---
    void SpawnEnemyWeapon();

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
    virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                      class AController* EventInstigator, AActor* DamageCauser) override;

    // --- Soul / Charm / Flee ---
    void SpawnSoul();
    void TriggerCharm(AActor* InPlayerActor);
    void BeginFlee();
    void DoNextFleeHop();
    void AddStateTag(const FGameplayTag& Tag);
    void RemoveStateTag(const FGameplayTag& Tag);
    FGenericTeamId GetGenericTeamId() const;

    // --- Helpers ---
    virtual int32 GetPlayerLevel() override;
    virtual void MulticastHandleDeath_Implementation() override;
    void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

    void SetEnemyState(EEnemyState NewState);          // [Restored]
    bool IsAttacking() const;                          // [Restored]
    bool IsPatrolling() const;                         // [Restored]
    bool bHitReacting = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float BaseWalkSpeed = 100.f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    EEnemyState EnemyState = EEnemyState::EES_Idle;
    
    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    UHAFAbilitySystemComponent* EnemyAbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    UHAFAttributeSet* EnemyAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
    UAttributeInfo* EnemyAttributeInfoOverride; // [Restored]

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* EnemyStatsWidgetComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
    UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
    UAISenseConfig_Sight* SightConfig;

    // --- Widgets ---
    UPROPERTY()
    UEnemyStatsWidget* EnemyStatsWidget;

    UPROPERTY()
    UEnemyAttributeMenuWidget* EnemyAttributeMenuWidget;

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

    // --- Flee / Charm ---
    UPROPERTY()
    AActor* CachedPlayer;

    bool bIsCharmed = false;
    bool bIsFleeing = false;

    UPROPERTY(EditDefaultsOnly, Category = "Flee")
    float FleeHopDistance = 800.f;

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

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEnemyStatsWidget> EnemyStatsWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEnemyAttributeMenuWidget> EnemyAttributeMenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Souls")
    TSubclassOf<ASoul> SoulClass;

    FGenericTeamId TeamId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FText EnemyDisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<AHAFAIController>HAFAIController;
    
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
    
protected:    
    void SafeInitASC_ForPawnOwner();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BindCallbacksToDependencies();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BroadcastInitialEnemyValues();

    UPROPERTY(BlueprintReadWrite)
    bool bDead = false;

   

private:
   
    
   

    bool bCanDamage = true;



    UPROPERTY(EditDefaultsOnly, Category="Combat")
    float BaseDamage = 20.f;

    // Death
   

   

    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* DeathParticles;

    UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* DeathSound;

   
    // Soul
    UPROPERTY(EditAnywhere, Category = "Soul")
    int32 SoulValue = 10;
    // Dissolve
    UPROPERTY()
    UTimelineComponent* DissolveTimeline;
    // Damage effect
    UPROPERTY(EditAnywhere, Category = "Combat|Damage")
    TSubclassOf<UGameplayEffect> DamageEffectClass;
   
    FTimerHandle DamageResetTimer;
    // Movement
   

    // Combat state

   
    // Cached player controller for UI
    UPROPERTY()
    APlayerController* CachedPC;
    // Hover state for UI
    UPROPERTY()
    bool bIsHovered = false;
    // Last hovered enemy for perception system
    UPROPERTY()
    AEnemyBase* LastHoveredEnemy = nullptr;
    // Weapons
   

    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* HitParticles;
    
private:
   
    /** Currently active hover menu widget instance */
    UPROPERTY()
    UEnemyAttributeMenuWidget* ActiveAttributeMenuWidget = nullptr;

   
public:
    FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }
    FORCEINLINE FText GetEnemyDisplayName() const { return EnemyDisplayName; }
};
