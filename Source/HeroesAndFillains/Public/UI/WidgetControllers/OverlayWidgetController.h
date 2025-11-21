// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/HAFWidgetController.h"
#include "GameplayEffectTypes.h"
#include "Items/CustomDesignedPCPickupItem.h"
#include "OverlayWidgetController.generated.h"

class UAbilityInfo;
struct FHAFAbilityInfo;
class UAttributeComponent;
class UCombatComponent;
class UHAFUserWidget;
class UHAFWidgetController;
class UDataTable;
class UHAFAbilitySystemComponent;
class UAbilitySystemComponent;
class ACustomDesignedPCPickupItem;
struct FUIWidgetRow;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message_A;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message_B;

	// If you want to reference a class/type (OK in DataTables):
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACustomDesignedPCPickupItem> EffectActorClass = nullptr;

	// Your message widget class (OK in DataTables)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UHAFUserWidget> MessageWidget = nullptr;

	// Prefer soft object ref for assets (hard refs are usually fine too, but soft is safer)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Image;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FHAFAbilityInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class HEROESANDFILLAINS_API UOverlayWidgetController : public UHAFWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// NEW: remember the last row tag we broadcast
	UPROPERTY(BlueprintReadOnly, Category="UI|Messages")
	FGameplayTag LastRowTag;

	// NEW: allows the overlay to "pull" the last message after it binds
	UFUNCTION(BlueprintCallable, Category="UI|Messages")
	void ReplayLastMessage();

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnMaxShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnStaminaChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnMajixChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnAttributeChangedSignature OnMaxMajixChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintReadOnly, Category="UI|Messages")
	int32 LastEffectActorLevel = 1;

	FORCEINLINE INT32 GetLastEffectActorLevel() const { return LastEffectActorLevel; }

	UPROPERTY() FGameplayTag LastBroadcastTag;
	UPROPERTY() double LastBroadcastTime = 0.0;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Ability Info")
	FAbilityInfoSignature AbilityInfoDelegate;

	void BroadcastAllAbilityInfo();

protected:
	UFUNCTION() // required for AddUObject binding
	void OnGEAddedToSelf(UAbilitySystemComponent* TargetASC,
						 const FGameplayEffectSpec& SpecApplied,
						 FActiveGameplayEffectHandle ActiveHandle);
protected:
	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetCurrentShield() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetCurrentMajix() const;

	UFUNCTION(BlueprintCallable, Category = "UI|VitalAttributes")
	float GetMaxMajix() const;

	void OnInitializeStartupAbilities(UHAFAbilitySystemComponent* HAFAbilitySystemComponent);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
		template<typename T>
		T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));;
}