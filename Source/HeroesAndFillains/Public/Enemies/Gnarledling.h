#pragma once

#include "EnemyBase.h"
#include "Gnarledling.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AGnarledling : public AEnemyBase
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
	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void EnableLeftFistlingMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void DisableLeftFistlingMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void EnableRightFistlingMeleeAttack();
	
	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void DisableRightFistlingMeleeAttack();
	
	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFistlingCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFistlingCollision;
	
	// Only Gnarled-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FistlingDamage = 10.f;

	FTimerHandle FistDamageResetTimer;

};
