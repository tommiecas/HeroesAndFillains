// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/WidgetControllers/HAFWidgetController.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/HAFEffectActor.h"
#include "OverlayWidgetController.generated.h"

class UAttributeComponent;
class UCombatComponent;
class UHAFUserWidget;
class UHAFWidgetController;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message_A = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AHAFEffectActor> EffectActorClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Level = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message_B = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UHAFUserWidget> MessageWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;

	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChangedSignature, float, NewShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxShieldChangedSignature, float, NewMaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChangedSignature, float, NewMaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMajixChangedSignature, float, NewMajix);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxMajixChangedSignature, float, NewMaxMajix);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

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

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnHealthChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnShieldChangedSignature OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnMaxShieldChangedSignature OnMaxShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnStaminaChangedSignature OnStaminaChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnMaxStaminaChangedSignature OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnMajixChangedSignature OnMajixChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Attributes")
	FOnMaxMajixChangedSignature OnMaxMajixChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Ability System | Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ShieldChanged(const FOnAttributeChangeData& Data) const;
	void MaxShieldChanged(const FOnAttributeChangeData& Data) const;
	void StaminaChanged(const FOnAttributeChangeData& Data) const;
	void MaxStaminaChanged(const FOnAttributeChangeData& Data) const;
	void MajixChanged(const FOnAttributeChangeData& Data) const;
	void MaxMajixChanged(const FOnAttributeChangeData& Data) const;

	
		template<typename T>
		T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));;
}