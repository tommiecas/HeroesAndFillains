// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "StormWeapons.generated.h"

class USoundBase;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AStormWeapons : public AMeleeWeapon
{
	GENERATED_BODY()
	
public:
	AStormWeapons();
	void SetupWeaponBox();
	virtual void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator) override;
	void OnAttackHit(const FHitResult& HitResult);
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void ClearDamagedActors();
	

	UPROPERTY()
	TSet<AActor*> DamagedActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AStormWeapons* StormWeapon;


	
protected:
	virtual void BeginPlay() override;
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:


public:

                      	
};