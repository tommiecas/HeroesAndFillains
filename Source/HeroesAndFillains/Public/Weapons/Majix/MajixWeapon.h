// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Weapons/WeaponBase.h"
#include "MajixWeapon.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AMajixWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AMajixWeapon();

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
