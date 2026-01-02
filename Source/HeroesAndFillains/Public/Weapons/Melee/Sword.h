// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API ASword : public AMeleeWeapon
{
	GENERATED_BODY()

public:
	ASword();
	virtual void WeaponDropped();
	void ResetSword();

protected:
	virtual void OnEquippedOneHanded();
	virtual void OnEquippedTwoHanded();
	virtual void OnDropped();
	virtual void BeginPlay() override;

private: 
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SwordMesh;

	

	FTransform InitialTransform;

public:
	FORCEINLINE FTransform GetInitialTransform() const { return InitialTransform; }
};
