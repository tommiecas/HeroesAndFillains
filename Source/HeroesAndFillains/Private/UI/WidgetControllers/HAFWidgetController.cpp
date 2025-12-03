// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/HAFWidgetController.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "UI/WidgetControllers/OverlayWidgetController.h"

void UHAFWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}



void UHAFWidgetController::BroadcastInitialValues()
{
	if (UOverlayWidgetController* OWC = Cast<UOverlayWidgetController>(this))
	{
		UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
		check(HAFAttributeSet);
		
		OWC->OnHealthChanged.Broadcast(HAFAttributeSet->GetHealth());
		OWC->OnMaxHealthChanged.Broadcast(HAFAttributeSet->GetMaxHealth());
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