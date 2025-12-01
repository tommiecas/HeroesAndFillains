// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Characters/CharacterClassInfo.h"
#include "Engine/DataAsset.h"
#include "EnemyInfo.generated.h"

enum class ECharacterClass : uint8;
class UGameplayEffect;
class AEnemyBase;

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Gnarled_Male,
	Gnarled_Female,
	StormAssassin_Sandstorm,
	StormAssassin_Soulstorm,
	StormAssassin_Skystorm,
	StormAssassin_Shadowstorm,
	Gnarledling,
	Thrope,
	Guul,
	Feretta,
	Hellspawn_Bloodspawn,
	Hellspawn_Netherspawn,
	Spectral_Ghost,
	Spectral_Marina,
	Spectral_Nova,
	Spectral_Scarlet,
	Spectral_Solaris,

	None
};

USTRUCT(BlueprintType)
struct FEnemyWiki
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "EnemyInfo")
	TSubclassOf<AEnemyBase> Enemy = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyInfo")
	FScalableFloat Damage = FScalableFloat();

	UPROPERTY(EditDefaultsOnly, Category = "EnemyInfo")
	FScalableFloat XPReward = FScalableFloat();
	
};

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EEnemyType EnemyType = EEnemyType::None;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyInfo")
	TMap<EEnemyType, FEnemyWiki> EnemyInformation;;

	FEnemyWiki GetEnemyInfo(EEnemyType EnType);


};

