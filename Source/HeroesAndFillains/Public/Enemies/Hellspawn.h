// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Hellspawn.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EHellspawnClass : uint8
{
	Bloodspawn,
	Netherspawn,
	Hellspawn,

	SpawnClass_MAX
};

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API AHellspawn : public AEnemyBase
{
	GENERATED_BODY()

public:
	AHellspawn();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHellspawnClass HellspawnClass = EHellspawnClass::SpawnClass_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHellspawnBloodspawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHellspawnNetherspawn;

	UFUNCTION(BlueprintCallable, Category="Combat|Zombie Feast")
	void StartZombieFeast();

	UFUNCTION(BlueprintCallable, Category="Combat|Zombie Feast")
	void StopZombieFeast();

	UPROPERTY(EditAnywhere, Category="Combat")
	TSubclassOf<UGameplayEffect> ZombieFeastDamageEffect;

	UFUNCTION(BlueprintCallable, Category="Combat|Zombie Feast")
	void BloodspawnTickBiteLerp();

	UFUNCTION(BlueprintCallable, Category="Combat|Zombie Feast")
	void NetherspawnTickBiteLerp();
	
	FTimerHandle BloodspawnBiteLerpTimer;
	FTimerHandle NetherspawnBiteLerpTimer;

	bool bIsLerpingToNeck = false;
	bool bIsLerpingToFoot = false;

	float BloodspawnBiteLerpDuration = 0.4f;  // adjustable, smoother/faster
	float BloodspawnBiteLerpElapsed = 0.f;
	float NetherspawnBiteLerpDuration = 0.2f;
	float NetherspawnBiteLerpElapsed = 0.f;
	
	FTransform NetherspawnBiteStartTransform;
	FTransform NetherspawnBiteTargetTransform;
	FTransform BloodspawnBiteStartTransform;
	FTransform BloodspawnBiteTargetTransform;
	
protected:
	void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void ResetCanDamage() override;
	virtual void Dissolve() override;

	virtual int32 PlayDeathMontage() override;

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void EnableTeethMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="Combat|Collision")
	void DisableTeethMeleeAttack();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UBoxComponent* TeethCollision;
	
	// Only Gnarled-specific settings remain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float TeethDamage = 8.f;
	
	FTimerHandle TeethDamageResetTimer;
};
