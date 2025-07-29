// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WidgetControllers/HAFWidgetController.h"

void UHAFWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UHAFWidgetController::BroadcastInitialValues()
{
}
