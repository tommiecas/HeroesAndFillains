// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "StormAssassin.generated.h"

UENUM(BlueprintType)
enum class EStormDeath : uint8
{
	ESD_Alive UMETA(DisplayName = "Alive"),
	ESD_Death1 UMETA(DisplayName = "Death1"),
	ESD_Death2 UMETA(DisplayName = "Death2"),
	EGD_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AStormAssassin : public AEnemyBase
{
	GENERATED_BODY()

public:
	AStormAssassin();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void EnemyDies() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	EStormDeath StormDeath = EStormDeath::ESD_Alive;	

	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual void PlayHitReactMontage(const FName& SectionName) override;
	virtual void PlayDeathMontage() override;

private:
	
	
};
