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
	for (const FGameplayTag&  Tag : TagContainer)
	{
		// TODO: Broadcast tag to the widget controller
		const FString Message = FString::Printf(TEXT("Gameplay Tag: %s"), *Tag.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Message);
	}
}
