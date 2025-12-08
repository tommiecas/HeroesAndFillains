// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "Interfaces/CombatInterface.h"
#include "HAFDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFDamageGameplayAbility : public UHAFGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);

protected:

	float GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
