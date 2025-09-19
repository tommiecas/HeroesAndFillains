// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Gnarled.h"
#include "Gnarledling.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AGnarledling : public AGnarled
{
	GENERATED_BODY()

public:
	AGnarledling();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnFistOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UFUNCTION()
	void HandleFistBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	virtual void ResetCanDamage() override;
	virtual int32 PlayDeathMontage() override;
	bool bCanDamage = true;;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LittleFistDamage = 10.f;
	
	virtual void EnableLeftFist() override;
	virtual void DisableLeftFist() override;
	virtual void EnableRightFist() override;
	virtual void DisableRightFist() override;
};
