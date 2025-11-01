#pragma once

#include "Enemies/EnemyBase.h"
#include "Thrope.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AThrope : public AEnemyBase
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
	virtual void EnableLeftSideMeleeAttack();
	virtual void DisableLeftSideMeleeAttack();
	virtual void EnableRightSideMeleeAttack();
	virtual void DisableRightSideMeleeAttack();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftHandClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFootClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightHandClawsCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFootClawsCollision;
	
	// Only Storm-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float HandClawsDamage = 35.f;
	float FootClawsDamage = 25.f;

	FTimerHandle HandClawsResetTimer;
	FTimerHandle FootClawsResetTimer;
};
