// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"
class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Majixian,
	Warrior,
	Gunslinger,
	Stealthian,
	Zealot
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;;
	
};
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterClassDefaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;;
	
	UPROPERTY(EditDefaultsOnly, Category = "ChommonClassDefaults")
	TSubclassOf<UGameplayEffect> InvisibleAttributes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CommonClassDefaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
	
};
