// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/AttributeMenuWidgetController.h"

#include "AbilitySystem/AttributeIhfo.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "Enemies/EnemyBase.h"
#include "PlayerState/HAFPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent || !AttributeSet) return;

	const UHAFAttributeSet* AS = CastChecked<UHAFAttributeSet>(AttributeSet);
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
	GetHAFPlayerState()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 NewAttributePoints)
		{
			OnAttributeMenuAttributePointsChangedDelegate.Broadcast(NewAttributePoints);
		}
	);	

}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UHAFAttributeSet* AS = CastChecked<UHAFAttributeSet>(AttributeSet);

	check (AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	OnAttributeMenuAttributePointsChangedDelegate.Broadcast(GetHAFPlayerState()->GetFillainPlayerAttributePoints());

}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FHAFAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UHAFAbilitySystemComponent* HAFASC = CastChecked<UHAFAbilitySystemComponent>(AbilitySystemComponent);
	HAFASC->UpgradeAttribute(AttributeTag);
}
