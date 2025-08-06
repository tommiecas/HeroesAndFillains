// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemComponent.h"

void UHAFAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UHAFAbilitySystemComponent::EffectApplied);
}

void UHAFAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
												   const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
	
	
}
