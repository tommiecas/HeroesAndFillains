// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot() const
{
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetLevelName(FString InLevelName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LevelName, InLevelName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

void UMVVM_LoadSlot::SetLoadSlotName(FString InLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);
}
