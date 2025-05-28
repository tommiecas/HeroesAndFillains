#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_None UMETA(DisplayName = "None"),
	
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	EWT_RubySword UMETA(DisplayName = "Ruby Sword"),
	EWT_SapphireSword UMETA(DisplayName = "Sapphire Sword"),
	EWT_ChaosSword UMETA(DisplayName = "Chaos Sword"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ERangedType : uint8
{
	ERT_None UMETA(DisplayName = "None"),
	
	ERT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	ERT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	ERT_Pistol UMETA(DisplayName = "Pistol"),
	ERT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	ERT_Shotgun UMETA(DisplayName = "Shotgun"),
	ERT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	ERT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),

	ERT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EMeleeType : uint8
{
	EMT_None UMETA(DisplayName = "None"),
	
	EMT_RubySword UMETA(DisplayName = "Ruby Sword"),
	EMT_SapphireSword UMETA(DisplayName = "Sapphire Sword"),
	EMT_ChaosSword UMETA(DisplayName = "Chaos Sword"),

	EMT_MAX UMETA(DisplayName = "DefaultMAX")
};