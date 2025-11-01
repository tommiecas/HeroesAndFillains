#pragma once

#include "Gnarled.h"
#include "Gnarledling.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AGnarledling : public AGnarled
{

	GENERATED_BODY()

public:
	AGnarledling();

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
	UBoxComponent* LeftFistlingCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFistlingCollision;
	
	// Only Gnarled-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FistlingDamage = 10.f;

	FTimerHandle FistlingDamageResetTimer;
};
