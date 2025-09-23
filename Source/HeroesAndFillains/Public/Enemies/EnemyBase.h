// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/EnemyInterface.h"
#include "Interfaces/HitInterface.h"
#include "Items/Soul.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "MotionWarpingComponent.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"
#include "Characters/CharacterClassInfo.h"
#include "EnemyBase.generated.h"

class UEnemyHealthBarWidget;
class UEnemyProgressBarBaseWidget;
class FOnAttributeChangedSignature;
class ARangedWeapon;
class AAIController;
class UWidgetComponent;

UENUM(BlueprintType, Blueprintable)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	EES_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HEROESANDFILLAINS_API AEnemyBase : public ABaseCharacter, public IEnemyInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/******************************
	****    Enemy Interface    ****
	******************************/
	
	virtual auto HighlightActor() -> void override;
	virtual void UnHighlightActor() override;

	/*******************************
	****    Combat Interface    ****
	*******************************/

	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bHighlighted = false;
	
	void SpawnEnemyWeapon();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UFUNCTION(BlueprintCallable)
	AAIController* LaunchEnemyAIController();

	void InitializeEnemy();
	virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void InitializeAbilitySystem();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Destroyed() override;
	virtual void AttackEnd() override;
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FString EnemyDisplayName = TEXT("Unnamed Enemy");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AMeleeWeapon* EquippedEnemyMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AWeaponBase* EquippedEnemyWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ARangedWeapon* EquippedEnemyRangedWeapon;

	UFUNCTION(BlueprintCallable)
	virtual void PlayRandomMeleeAttackMontage() override;
	virtual void PlayRandomMajixAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 DeadEnemySoulCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintCallable)
	void TriggerCharm(AActor* InPlayerActor);
	
	// IGenericTeamAgentInterface
	virtual FGenericTeamId GetGenericTeamId() const override; // declaration only
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override { TeamId = InTeamId; }

	// Called by BT Service when no enemy remains or player dead
	UFUNCTION(BlueprintCallable)
	void BeginFlee();

	// Accessors the BT will use:
	bool IsCharmed() const { return bIsCharmed; }
	bool IsFleeing() const { return bIsFleeing; }
	AActor* GetCachedPlayer() const { return CachedPlayer; }

	// Recompute next hop after reaching last hop
	void DoNextFleeHop();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UEnemyProgressBarBaseWidget> EnemyHealthBarWidgetClass;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) const;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bReactingToHit = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bStartupAbilitiesGranted = false;
	
	FDelegateHandle HealthChangedHandle;
	FDelegateHandle MaxHealthChangedHandle;
	FDelegateHandle HitReactChangedHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> EnemyHealthBar;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bASCBindingsInitialized = false;




	virtual void InitializeAbilityActorInfo() override;
	void SpawnSoul();
	virtual void CharacterDies() override;
	virtual void PlayAttackMontage() override;
	virtual int32 PlayDeathMontage() override;
	virtual void MeleeAttack() override;
	virtual void MajixAttack() override;
	virtual bool CanAttack() override;

	
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;
	
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;
	
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void ClearPatrolTimer();

	void PatrolTimerFinished();
	void EnemiesLoseInterest();
	void EnemiesStartPatrolling();
	bool IsOutsideCombatRadius();
	void EnemiesChaseTarget();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsEnemyDead();
	bool IsEnemyChasing();
	bool IsEnemyAttacking();
	bool IsEnemyEngaged();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void StartAttackTimer();
	void ClearAttackTimer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsCharmed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFleeing = false;

	UPROPERTY()
	AActor* CachedPlayer = nullptr;

	// Team ids: 0 = Enemy (default), 1 = PlayerAlly (charmed)
	FGenericTeamId TeamId = FGenericTeamId(0);

	// How far to run each hop while fleeing
	UPROPERTY(EditDefaultsOnly, Category="Charm")
	float FleeHopDistance = 5000.f;

	

	// Helper: apply/remove GAS tags if you’re on GAS
	void AddStateTag(const FGameplayTag& Tag);
	void RemoveStateTag(const FGameplayTag& Tag);

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 3.f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = .5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	
	/*********************
	***                ***
	***   NAVIGATION   ***
	***                ***
	*********************/

	UPROPERTY(VisibleAnywhere)
	class AAIController* EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double PatrolRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
private:
	

public:
	FORCEINLINE FString GetEnemyDisplayName() const { return EnemyDisplayName; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	

};