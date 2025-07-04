// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	EES_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HEROESANDFILLAINS_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();
	void SpawnEnemyWeapon();
	void HideHealthBarWidget();
	void LaunchEnemyAIController();
	void InitializeEnemy();
	virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void Destroyed() override;
	virtual void AttackEnd() override;
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AMeleeWeapon* EquippedEnemyMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AWeaponBase* EquippedEnemyWeapon;

	
protected:
	virtual void BeginPlay() override;
	virtual void CharacterDies() override;
	virtual int32 PlayDeathMontage() override;
	virtual int32 PlayMeleeAttackMontage() override;
	virtual void MeleeAttack() override;
	virtual bool CanAttack() override;
	virtual float CalculateDamage(AActor* DamagedPawn, float DamageAmount, AController* InstigatorController) override;

	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) override;

	
	



	


	

	
private:
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
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
	void HideHealthBar();
	void ShowHealthBar();
	void StartAttackTimer();
	void ClearAttackTimer();
	
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 6.f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = .5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;
	
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;

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

public:
	

};
