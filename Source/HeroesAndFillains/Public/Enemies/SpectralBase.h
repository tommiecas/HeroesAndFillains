// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyBase.h"
#include "SpectralBase.generated.h"

UCLASS()
class HEROESANDFILLAINS_API ASpectralBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	ASpectralBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnAttackCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	virtual void Dissolve() override;
	virtual int32 PlayDeathMontage() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> SpectralAssaultRifle;

protected:
	
	
};
