// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WidgetControllers/OverlayWidgetController.h"

#include "AbilitySystem/HAFAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(HAFAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(HAFAttributeSet->GetMaxHealth());
}
