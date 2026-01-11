#pragma once

#include "CoreMinimal.h"
#include "HaFGameMode.h"
#include "GameModeTypes.h"
#include "HAFHybridGameMode.generated.h"

class UHAFSaveGame;
class UMVVM_LoadSlot;
class USaveGame;

UCLASS()
class HEROESANDFILLAINS_API AHAFHybridGameMode : public AHAFGameMode
{
	GENERATED_BODY()

public:
	AHAFHybridGameMode();

	/** Current active mode */
	UPROPERTY(BlueprintReadOnly, Category="Game Mode")
	EGameModeType CurrentMode;

	/** Switch between PvE and PvP */
	UFUNCTION(BlueprintCallable, Category="Game Mode")
	void SetGameMode(EGameModeType NewMode);

	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable)
	UHAFSaveGame* RetrieveInGameSaveData();

	UFUNCTION(BlueprintCallable)
	void SaveInGameProgressData(UHAFSaveGame* SaveObject);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Game")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UFUNCTION(BlueprintCallable)
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	UHAFSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable)
	void TravelToMap(UMVVM_LoadSlot* LoadSlot);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void SaveWorldState(UWorld* World) const;
	void LoadWorldState(UWorld* World) const;

protected:
	/** Called when switching to PvE */
	virtual void OnEnterPvE() override;

	/** Called when switching to PvP */
	virtual void OnEnterPvP() override;
};
