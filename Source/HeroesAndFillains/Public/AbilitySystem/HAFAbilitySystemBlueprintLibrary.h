// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Characters/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetControllers/HAFWidgetController.h"
#include "HAFAbilitySystemBlueprintLibrary.generated.h"

class AFillainHUD;
class USpellMenuWidgetController;
enum class EEnemyType : uint8;
class UEnemyInfo;
class AEnemyBase;
class UEnemyAttributeMenuWidgetController;
class UFillainAttributeMenuWidgetController;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AFillainHUD*& OutFillainHUD);
	
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
		UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
    	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "HAFAbilitySystemBlueprintLibrary | Character Class Defaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "HAFAbilitySystemBlueprintLibrary | Character Class Defaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="HAFAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="HAFAbilitySystemLibrary|EnemyDefaults")
	static UEnemyInfo* GetEnemyInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="HAFAbilitySystemLibrary|AbilityDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	static FGameplayEffectContextHandle AddSourceObjectToContext(
		const FGameplayEffectContextHandle& Context, 
		UObject* SourceObject);

	UFUNCTION(BlueprintPure, Category="HAF AbilitySystem Library | GameplayEffects")
	static bool IsGameplayEffectSpecHandleValid(const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const TArray<FVector>& SphereOrigins);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	static int32 GetXPAwardForEnemyTypeAndLevel(const UObject* WorldContextObject, EEnemyType EnemyType, int32 EnemyLevel);
	
};
