#pragma once

#include "CoreMinimal.h"
#include "HaFGameMode.h"
#include "GameModeTypes.h"
#include "HybridGameMode.generated.h"

class ULoadScreenSaveGame;
class UMVVM_LoadSlot;
class USaveGame;

UCLASS()
class HEROESANDFILLAINS_API AHybridGameMode : public AHAFGameMode
{
	GENERATED_BODY()

public:
	AHybridGameMode();

	/** Current active mode */
	UPROPERTY(BlueprintReadOnly, Category="Game Mode")
	EGameModeType CurrentMode;

	/** Switch between PvE and PvP */
	UFUNCTION(BlueprintCallable, Category="Game Mode")
	void SetGameMode(EGameModeType NewMode);

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Game")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UFUNCTION(BlueprintCallable)
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable)
	void TravelToMap(UMVVM_LoadSlot* LoadSlot);
	
protected:
	/** Called when switching to PvE */
	virtual void OnEnterPvE() override;

	/** Called when switching to PvP */
	virtual void OnEnterPvP() override;
};
