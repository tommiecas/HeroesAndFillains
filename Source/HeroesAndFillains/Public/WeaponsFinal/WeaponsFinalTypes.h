#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponFinalType : uint8
{
	EWFT_None UMETA(DisplayName = "None"),

	EWFT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWFT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWFT_Pistol UMETA(DisplayName = "Pistol"),
	EWFT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWFT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWFT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWFT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	EWFT_Sword UMETA(DisplayName = "Sword"),


	EWFT_MAX UMETA(DisplayName = "DefaultMAX")
};
