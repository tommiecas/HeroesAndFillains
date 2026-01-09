// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "GameMode/LoadScreenSaveGame.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	virtual void BeginDestroy() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString EnteredName);

	UFUNCTION(BlueprintCallable)
	void AddNewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	void LoadData();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;

	UFUNCTION(BlueprintCallable)
	void FinalDeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed() const;

private:
	UPROPERTY()
	UMVVM_LoadSlot* SelectedSlot;

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
	
	
};
