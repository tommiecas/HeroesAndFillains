// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HAFDamageGameplayAbility.h"
#include "HAFFireBlast.generated.h"

class AHAFFireball;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFFireBlast : public UHAFDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<AHAFFireball*> SpawnFireballs();
	
	virtual FString GetDescription(int32 Level)	override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FireBlast")
	int32 NumFireballs = 12;

private:
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	TSubclassOf<AHAFFireball> FireballClass;
};
