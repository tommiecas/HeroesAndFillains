#pragma once

UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	EGMT_PvE UMETA(DisplayName = "Player vs Environment"),
	EGMT_PvP UMETA(DisplayName = "Player vs Player")
};