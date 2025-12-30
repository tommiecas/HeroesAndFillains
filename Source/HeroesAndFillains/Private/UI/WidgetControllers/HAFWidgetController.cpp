// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/HAFWidgetController.h"

#include "AbilitySystem/AbilityInfo.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "PlayerController/FillainPlayerController.h"
#include "PlayerState/HAFPlayerState.h"
#include "UI/WidgetControllers/OverlayWidgetController.h"

void UHAFWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UHAFWidgetController::BroadcastAbilityInfo()
{
	if (!GetHAFAbilitySystemComponent()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FHAFAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetHAFAbilitySystemComponent()->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = GetHAFAbilitySystemComponent()->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = HAFAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetHAFAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
}


void UHAFWidgetController::BroadcastInitialValues()
{
	if (UOverlayWidgetController* OWC = Cast<UOverlayWidgetController>(this))
	{
		UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet);
		check(HAFAttSet);
		
		OWC->OnHealthChanged.Broadcast(HAFAttSet->GetHealth());
		OWC->OnMaxHealthChanged.Broadcast(HAFAttSet->GetMaxHealth());
		OWC->OnShieldChanged.Broadcast(HAFAttributeSet->GetShield());
		OWC->OnMaxShieldChanged.Broadcast(HAFAttributeSet->GetMaxShield());
		OWC->OnStaminaChanged.Broadcast(HAFAttributeSet->GetStamina());
		OWC->OnMaxStaminaChanged.Broadcast(HAFAttributeSet->GetMaxStamina());
		OWC->OnMajixChanged.Broadcast(HAFAttributeSet->GetMajix());
		OWC->OnMaxMajixChanged.Broadcast(HAFAttributeSet->GetMaxMajix());
	}
	else return;
}

void UHAFWidgetController::BindCallbacksToDependencies()
{
}

AFillainPlayerController* UHAFWidgetController::GetFillainPlayerController()
{
	if (FillainPlayerController == nullptr)
	{
		FillainPlayerController = Cast<AFillainPlayerController>(PlayerController);
	}
	return FillainPlayerController;
}

AHAFPlayerState* UHAFWidgetController::GetHAFPlayerState()
{
	if (HAFPlayerState == nullptr)
	{
		HAFPlayerState = Cast<AHAFPlayerState>(PlayerState);
	}
	return HAFPlayerState;
}

UHAFAbilitySystemComponent* UHAFWidgetController::GetHAFAbilitySystemComponent()
{
	if (HAFAbilitySystemComponent == nullptr)
	{
		HAFAbilitySystemComponent = Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent);
	}
	return HAFAbilitySystemComponent;
}

UHAFAttributeSet* UHAFWidgetController::GetHAFAttributeSet()
{
	if (HAFAttributeSet == nullptr)
	{
		HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	}
	return HAFAttributeSet;
}
