#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "HAFGameMode.generated.h"

class UCharacterClassInfo;

namespace MatchState
{
	extern HEROESANDFILLAINS_API const FName Cooldown;
};

/**
 * Base GameMode for Heroes and Fillains
 */
UCLASS(BlueprintType, Blueprintable)
class HEROESANDFILLAINS_API AHAFGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AHAFGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AFillainCharacter* VictimCharacter, class AFillainPlayerController* VictimController, AFillainPlayerController* KillerController);
	virtual void RequestRespawn(ACharacter* VictimCharacter, AController* VictimController);
	void PlayerLeftGame(class AHAFPlayerState* LeavingPlayer);
	virtual float CalculateDamage(AController* Killer, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
    
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float LevelStartingTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	void SendChat(const FString& Text, const FString& PlayerName);

	bool bTeamsMatch = false;
	bool bDelayedStart = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	/** New: Hooks for HybridGameMode (safe to call Super) */
	virtual void OnEnterPvE();
	virtual void OnEnterPvP();

private:
	UPROPERTY()
	AFillainCharacter* Character;

	float CountdownTime = 0.f;

public:
	class AFillainCharacter* GetCharacter() const { return Character; }
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
