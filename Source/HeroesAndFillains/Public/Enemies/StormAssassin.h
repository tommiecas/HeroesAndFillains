#pragma once

#include "CombatCollisionsAmendedEnemyBaseClass.h"
#include "StormAssassin.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AStormAssassin : public ACombatCollisionsAmendedEnemyBaseClass
{

	GENERATED_BODY()

public:
	AStormAssassin();

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
	UBoxComponent* LeftFootCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFootCollision;
	
	// Only Storm-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FootDamage = 25.f;

	FTimerHandle FootDamageResetTimer;
};
