// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "HAFDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFDamageGameplayAbility : public UHAFGameplayAbility
{
	GENERATED_BODY()

public:

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
