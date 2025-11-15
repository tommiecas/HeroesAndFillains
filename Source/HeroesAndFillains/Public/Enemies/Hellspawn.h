// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Hellspawn.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AHellspawn : public AEnemyBase
{
	GENERATED_BODY()

public:
	AHellspawn();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void ResetCanDamage() override;
	virtual void Dissolve() override;

	virtual int32 PlayDeathMontage() override;

};
