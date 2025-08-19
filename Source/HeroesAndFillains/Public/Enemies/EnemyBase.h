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
#include "EnemyBase.generated.h"

class ARangedWeapon;
class AAIController;
class UEnemyHealthBarWidgetComponent;
class UEnemyHealthBarWidget;

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
class HEROESANDFILLAINS_API AEnemyBase : public ABaseCharacter, public IEnemyInterface
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
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bHighlighted = false;
	
	void SpawnEnemyWeapon();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<UAnimMontage*> MeleeAttackMontages;

	UFUNCTION(BlueprintCallable)
	virtual void PlayRandomAttackMontage();

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

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	void SpawnSoul();
	virtual void CharacterDies() override;
	virtual int32 PlayMeleeAttackMontage() override;
	virtual int32 PlayDeathMontage() override;
	virtual void MeleeAttack() override;
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
	void HideHealthBarWidgetComponent();
	void ShowHealthBarWidgetComponent();
	void StartAttackTimer();
	void ClearAttackTimer();
	
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
private:
	

public:
	FORCEINLINE FString GetEnemyDisplayName() const { return EnemyDisplayName; }
	

};