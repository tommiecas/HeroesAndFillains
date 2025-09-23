// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HAFAbilitySystemBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "HAFAbilitySystemBlueprintLibrary | Character Class Defaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "HAFAbilitySystemBlueprintLibrary | Character Class Defaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	static FGameplayEffectContextHandle AddSourceObjectToContext(
		const FGameplayEffectContextHandle& Context, 
		UObject* SourceObject);

	UFUNCTION(BlueprintPure, Category="Ability|GameplayEffects")
	static bool IsGameplayEffectSpecHandleValid(const FGameplayEffectSpecHandle& SpecHandle);
	
};
