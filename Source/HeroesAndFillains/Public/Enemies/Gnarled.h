#pragma once

#include "Enemies/EnemyBase.h"
#include "Gnarled.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AGnarled : public AEnemyBase
{

	GENERATED_BODY()

public:
	AGnarled();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGnarledMale = true;
	
protected:
	void EnableLeftSideMeleeAttack();
	void DisableLeftSideMeleeAttack();
	void EnableRightSideMeleeAttack();
	void DisableRightSideMeleeAttack();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFistCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFistCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFootCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFootCollision;
	
	// Only Gnarled-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FistDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FootDamage = 25.f;

	FTimerHandle FistDamageResetTimer;
	FTimerHandle FootDamageResetTimer;

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineOne(UMaterialInstanceDynamic* DynamicMaterialInstanceOne);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineTwo(UMaterialInstanceDynamic* DynamicMaterialInstanceTwo);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceOne;
        
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceTwo;

};
