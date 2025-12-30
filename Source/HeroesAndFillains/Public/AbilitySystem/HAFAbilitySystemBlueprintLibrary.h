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

	/********************************
	*********************************
	****    WIDGET CONTROLLER    ****
	****    LIBRARY FUNCTIONS    ****
	*********************************
	********************************/
	
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AFillainHUD*& OutFillainHUD);
	
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "HAFAbilitySystemBlueprintLibrary | Widget Controllers", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	

	/*********************************
	**********************************
	****      ABILITY SYSTEM      ****
	****      CLASS DEFAULTS      ****
	****     LIBRARY FUNCTIONS    ****
	**********************************
	*********************************/
	
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

	/**************************
	***************************
	****       EFFECT      ****
	****      CONTEXT      ****
	****       GETTER      ****
	****      LIBRARY      ****
	****     FUNCTIONS     ****
	***************************
	**************************/
	
	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsDebuffSuccessful(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static FGameplayTag GetDamageTypeTag(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="HAF Ability System Library | GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	
	/**************************
	***************************
	****       EFFECT      ****
	****      CONTEXT      ****
	****       SETTER      ****
	****      LIBRARY      ****
	****     FUNCTIONS     ****
	***************************
	**************************/
	
	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsDebuffSuccessful(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDebuffSuccessful);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float Damage);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float Duration);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float Frequency);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetDamageTypeTag(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageTypeTag);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector InImpulse);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector InForce);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);
	
	/*********************************
	**********************************
	****    GAMEPLAY MECHANICS    ****
	****    LIBRARY FUNCTIONS     ****
	**********************************
	*********************************/
	
	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category="HAF Ability System Library | GameplayEffects")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="HAF Ability System Library | GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);
		
	static int32 GetXPAwardForEnemyTypeAndLevel(const UObject* WorldContextObject, EEnemyType EnemyType, int32 EnemyLevel);
	
};
