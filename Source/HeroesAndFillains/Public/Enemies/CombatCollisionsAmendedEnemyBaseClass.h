// EnemyCombatBase.h
#pragma once
#include "Enemies/EnemyBase.h"
#include "CombatCollisionsAmendedEnemyBaseClass.generated.h"

class UBoxComponent;

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API ACombatCollisionsAmendedEnemyBaseClass : public AEnemyBase
{
	GENERATED_BODY()

public:
	ACombatCollisionsAmendedEnemyBaseClass();

	virtual void BeginPlay() override;

	/** Registers a limb collision box */
	virtual void RegisterAttackCollision(UBoxComponent* CollisionBox) override;

	/** Enables / disables collisions for all boxes */
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) override;

	// Virtual, empty by default — so each enemy can override
	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	virtual void EnableLeftSideMeleeAttack() {}

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	virtual void DisableLeftSideMeleeAttack() {}

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	virtual void EnableRightSideMeleeAttack() {}

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	virtual void DisableRightSideMeleeAttack() {}
	
protected:
	UFUNCTION()
	virtual void OnAttackCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	virtual void ResetCanDamage() override;
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void MeleeAttack() override;


	bool bCanDamage = true;
	// ✅ How much base damage a single melee hit deals
	float BaseDamage = 20.f;
	FTimerHandle DamageResetTimer;
	
};
