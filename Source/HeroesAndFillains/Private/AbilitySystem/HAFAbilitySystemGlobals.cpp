// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemGlobals.h"

#include "HAFAbilityTypes.h"

FGameplayEffectContext* UHAFAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FHAFGameplayEffectContext();
}
