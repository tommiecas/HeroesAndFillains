// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HAFProjectileSpell.h"
#include "HAFFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFFireBolt : public UHAFProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	
};
