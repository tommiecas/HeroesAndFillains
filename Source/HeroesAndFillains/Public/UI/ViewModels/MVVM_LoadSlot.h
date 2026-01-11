// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "GameMode/HAFSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);


/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	void InitializeSlot() const;

	

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	FName PlayerStartTag;

	/** Field Notifies **/

	

	void SetPlayerName(FString InPlayerName);
	void SetLevelName(FString InLevelName);
	void SetPlayerLevel(int32 InPlayerLevel);
	void SetLoadSlotName(FString InLoadSlotName);
	
	FString GetPlayerName() const { return PlayerName; }
	FString GetLevelName() const { return LevelName; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	FString GetLoadSlotName() const { return LoadSlotName; }

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"));
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, meta = (AllowPrivateAccess = "true"))
	int32 PlayerLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, meta = (AllowPrivateAccess = "true"))
	FString LoadSlotName;
};
