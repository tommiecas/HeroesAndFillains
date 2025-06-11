// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "ChaosSword.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AChaosSword : public AMeleeWeapon
{
	GENERATED_BODY()

public:
	AChaosSword();
	virtual void BeginPlay() override;
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void OnEquippedTwoHanded() override;
	virtual void OnRep_WeaponState() override;

	virtual void Equip(USceneComponent* InParent, FName InSocketName) override;
};
