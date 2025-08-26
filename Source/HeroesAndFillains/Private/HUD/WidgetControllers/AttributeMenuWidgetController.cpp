// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WidgetControllers/AttributeMenuWidgetController.h"

#include "AbilitySystem/AttributeIhfo.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "HAFGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UHAFAttributeSet* AS = CastChecked<UHAFAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
		[this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		}
	);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UHAFAttributeSet* AS = CastChecked<UHAFAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FHAFAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
