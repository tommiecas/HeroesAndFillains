// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Guul.generated.h"

class UAISenseConfig_Hearing;

UCLASS()
class HEROESANDFILLAINS_API AGuul : public AEnemyBase
{
	GENERATED_BODY()

public:
	AGuul();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
	UAISenseConfig_Hearing* HearingConfig;
	
protected:
	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void EnableLeftFistMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void DisableLeftFistMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void EnableRightFistMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void DisableRightFistMeleeAttack();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* LeftFistCollision;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* RightFistCollision;
	
	// Only Gnarled-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float FistDamage = 12.f;
	
	FTimerHandle FistDamageResetTimer;


};
