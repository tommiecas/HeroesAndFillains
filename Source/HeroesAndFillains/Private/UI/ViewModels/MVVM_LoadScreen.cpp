// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/MVVM_LoadScreen.h"

#include "GameMode/HybridGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModels/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString EnteredName)
{
	AHybridGameMode* HAFHybridGameMode = Cast<AHybridGameMode>(UGameplayStatics::GetGameMode(this));
	
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->SetLevelName(HAFHybridGameMode->DefaultLevelName);
	
	HAFHybridGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();
}

void UMVVM_LoadScreen::AddNewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);

		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::LoadData()
{
	AHybridGameMode* HAFHybridGameMode = Cast<AHybridGameMode>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveObject = HAFHybridGameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);
		const FString PlayerName = SaveObject->PlayerName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->InitializeSlot();
		LoadSlot.Value->SetLevelName(SaveObject->LevelName);
	}
}

void UMVVM_LoadScreen::FinalDeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AHybridGameMode::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed() const
{
	AHybridGameMode* HAFHybridGameMode = Cast<AHybridGameMode>(UGameplayStatics::GetGameMode(this));
	if (IsValid(SelectedSlot))
	{
		HAFHybridGameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::BeginDestroy()
{
	UE_LOG(LogTemp, Error, TEXT("LoadScreenViewModel BEGIN DESTROY"));
	Super::BeginDestroy();
}
