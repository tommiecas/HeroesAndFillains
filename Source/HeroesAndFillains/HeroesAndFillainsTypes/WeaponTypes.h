#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType, Blueprintable)
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
	EWT_ShadowSword UMETA(DisplayName = "ShadowSword"),
	EWT_SandSword UMETA(DisplayName = "SandSword"),
	EWT_SoulSword UMETA(DisplayName = "SoulSword"),
	EWT_SkyMace UMETA(DisplayName = "SkyMace"),
	EWT_MajixProjectile UMETA(DisplayName = "MajixProjectile"),
	EWT_FireBolt UMETA(DisplayName = "FireBolt"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType, Blueprintable)
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

UENUM(BlueprintType, Blueprintable)
enum class EMeleeType : uint8
{
	EMT_None UMETA(DisplayName = "None"),
	
	EMT_RubySword UMETA(DisplayName = "Ruby Sword"),
	EMT_SapphireSword UMETA(DisplayName = "Sapphire Sword"),
	EMT_ChaosSword UMETA(DisplayName = "Chaos Sword"),
	EWT_ShadowSword UMETA(DisplayName = "ShadowSword"),
	EMT_SandSword UMETA(DisplayName = "SandSword"),
	EMT_SoulSword UMETA(DisplayName = "SoulSword"),
	EMT_SkyMace UMETA(DisplayName = "SkyMace"),

	EMT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType, Blueprintable)
enum class EMaJixType : uint8
{
	EJT_None UMETA(DisplayName = "None"),
	
	EJT_MajixProjectile UMETA(DisplayName = "MajixProjectile"),
	EJT_FireBolt UMETA(DisplayName = "FireBolt"),

	EJT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType, Blueprintable)
enum class EHandsNeeded : uint8
{
	EHN_None UMETA(DisplayName = "No Hands Needed"),
	EHN_OneHandedWeapon UMETA(DisplayName = "One Handed Weapon"),
	EHN_TwoHandedWeapon UMETA(DisplayName = "Two Handed Weapon"),

	EHN_MAX UMETA(DisplayName = "DefaultMAX")
};