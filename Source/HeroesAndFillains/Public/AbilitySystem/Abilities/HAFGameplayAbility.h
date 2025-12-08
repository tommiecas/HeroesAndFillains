// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HAFGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 Level);
	virtual FString GetNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);

	virtual float GetMajixCost(float InLevel = 1.f);
	virtual float GetCooldown(float InLevel = 1.f);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	int32 GetSafeAbilityLevel() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	float GetScalableModifierMagnitude(TSubclassOf<UGameplayEffect> GEClass, const FGameplayAttribute& Attribute, float AbilityLevel);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	virtual float GetAbilityCost(const FGameplayAttribute& Attribute) const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Cooldown")
	virtual float GetAbilityCooldown() const;

	UFUNCTION(BlueprintCallable)
	virtual float CalculateAbilityCost(int32 CharacterLevel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbilityCost = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbilityCooldown = 0.5f;

	FORCEINLINE float GetCost() const { return AbilityCost; }
	FORCEINLINE void SetCost(float NewCost) { AbilityCost = NewCost; }
	FORCEINLINE float GetCooldown() const { return AbilityCooldown; }
	FORCEINLINE void SetCooldown(float NewCooldown) { AbilityCooldown = NewCooldown; }
};
