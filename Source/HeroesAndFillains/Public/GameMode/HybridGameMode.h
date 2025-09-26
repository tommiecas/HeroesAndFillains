#pragma once

#include "CoreMinimal.h"
#include "HaFGameMode.h"
#include "GameModeTypes.h"
#include "HybridGameMode.generated.h"

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

protected:
	/** Called when switching to PvE */
	virtual void OnEnterPvE() override;

	/** Called when switching to PvP */
	virtual void OnEnterPvP() override;
};
