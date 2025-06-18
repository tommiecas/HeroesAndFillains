// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Interfaces/HitInterface.h"
#include "Gnarled.generated.h"

UENUM(BlueprintType)
enum class EGnarledDeath : uint8
{
	EGD_Alive UMETA(DisplayName = "Alive"),
	EGD_Death1 UMETA(DisplayName = "Death1"),
	EGD_Death2 UMETA(DisplayName = "Death2"),
	EGD_Death3 UMETA(DisplayName = "Death3"),
	EGD_Death4 UMETA(DisplayName = "Death4"),
	EGD_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AGnarled : public AEnemyBase
{
	GENERATED_BODY()

public:
	AGnarled();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void EnemyDies() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	EGnarledDeath GnarledDeath = EGnarledDeath::EGD_Alive;	

	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual void PlayHitReactMontage(const FName& SectionName) override;
	virtual void PlayDeathMontage() override;

private:
	
	
};
