// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/MVVM_LoadScreen.h"

#include "CommonUIUtils.h"
#include "GameMode/HAFGameInstance.h"
#include "GameMode/HAFHybridGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModels/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);

	SetNumLoadSlots(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString EnteredName)
{
	AHAFHybridGameMode* HAFHybridGameMode = Cast<AHAFHybridGameMode>(UGameplayStatics::GetGameMode(this));
	
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->SetLevelName(HAFHybridGameMode->DefaultLevelName);
	LoadSlots[Slot]->PlayerStartTag = HAFHybridGameMode->DefaultPlayerStartTag;
	
	HAFHybridGameMode->SaveSlotData(LoadSlots[Slot], Slot);
	LoadSlots[Slot]->InitializeSlot();

	if (!HAFHybridGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("NewSlotButtonPressed: GameMode is NULL"));
		return;
	}

	UHAFGameInstance* HAFGameInstance =
		Cast<UHAFGameInstance>(HAFHybridGameMode->GetGameInstance());

	if (!HAFGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("NewSlotButtonPressed: GameInstance is NULL"));
		return;
	}

	if (!(IsValid(LoadSlots[Slot])))
	{
		UE_LOG(LogTemp, Error, TEXT("NewSlotButtonPressed: LoadSlots[%d] is NULL"), Slot);
		return;
	}

	HAFGameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	HAFGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	HAFGameInstance->PlayerStartTag = HAFHybridGameMode->DefaultPlayerStartTag;
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
	AHAFHybridGameMode* HAFHybridGameMode = Cast<AHAFHybridGameMode>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		UHAFSaveGame* SaveObject = HAFHybridGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);
		const FString PlayerName = SaveObject->PlayerName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->InitializeSlot();
		LoadSlot.Value->SetLevelName(SaveObject->LevelName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}

void UMVVM_LoadScreen::FinalDeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AHAFHybridGameMode::DeleteSlot(SelectedSlot->GetLoadSlotName(), SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed() const
{
	AHAFHybridGameMode* HAFHybridGameMode = Cast<AHAFHybridGameMode>(UGameplayStatics::GetGameMode(this));
	UGameInstance* GI = HAFHybridGameMode->GetGameInstance();

	UE_LOG(LogTemp, Warning,
		TEXT("Raw GameInstance: %s | Class: %s"),
		*GetNameSafe(GI),
		GI ? *GetNameSafe(GI->GetClass()) : TEXT("NULL"));
	
	UHAFGameInstance* HAFGI = Cast<UHAFGameInstance>(GI);

	UE_LOG(LogTemp, Warning,
		TEXT("GI: %s | Cast to UHAFGameInstance: %s"),
		*GetNameSafe(GI),
		HAFGI ? TEXT("SUCCESS") : TEXT("FAILED"));

	UWorld* World = HAFHybridGameMode->GetWorld();

	UE_LOG(LogTemp, Warning,
		TEXT("World: %s | GI Class: %s"),
		*GetNameSafe(World),
		World && World->GetGameInstance()
			? *GetNameSafe(World->GetGameInstance()->GetClass())
			: TEXT("NULL"));

	
	UHAFGameInstance* HAFGameInstance = Cast<UHAFGameInstance>(HAFHybridGameMode->GetGameInstance());
	HAFGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	HAFGameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
	HAFGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;

	if (IsValid(SelectedSlot))
	{
		HAFHybridGameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumLoadSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InNumLoadSlots);
}

