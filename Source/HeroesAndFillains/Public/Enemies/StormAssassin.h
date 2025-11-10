#pragma once

#include "Enemies/EnemyBase.h"
#include "StormAssassin.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AStormAssassin : public AEnemyBase
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
	virtual void EnableLeftSideMeleeAttack();
	virtual void DisableLeftSideMeleeAttack();
	virtual void EnableRightSideMeleeAttack();
	virtual void DisableRightSideMeleeAttack();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFootCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFootCollision;
	
	// Only Storm-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FootDamage = 25.f;

	FTimerHandle FootDamageResetTimer;
};
