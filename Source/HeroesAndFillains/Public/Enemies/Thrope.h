#pragma once

#include "CombatCollisionsAmendedEnemyBaseClass.h"
#include "StormAssassin.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AThrope : public ACombatCollisionsAmendedEnemyBaseClass
{

	GENERATED_BODY()

public:
	AThrope();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnAttackCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
	virtual void Dissolve() override;
	virtual int32 PlayDeathMontage() override;

protected:
	virtual void EnableLeftSideMeleeAttack() override;
	virtual void DisableLeftSideMeleeAttack() override;
	virtual void EnableRightSideMeleeAttack() override;
	virtual void DisableRightSideMeleeAttack() override;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftHandsClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFeetClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightHandsClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFeetClawsCollision;
	
	// Only Storm-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float ClawsHandDamage = 25.f;
	float ClawsFeettDamage = 25.f;

	FTimerHandle ClawsHandsResetTimer;
	FTimerHandle ClawsFeetResetTimer;
};
