// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "StormAssassin.generated.h"


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
	virtual void CharacterDies() override;
	virtual void MeleeAttack() override;
	virtual void AttackEnd() override;
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) override;



protected:
	virtual void BeginPlay() override;
	
	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual void PlayHitReactMontage(const FName& SectionName) override;
	virtual int32 PlayDeathMontage() override;
	virtual int32 PlayMeleeAttackMontage() override;

private:
	
	
};
