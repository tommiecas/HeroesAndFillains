// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "HAFAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
