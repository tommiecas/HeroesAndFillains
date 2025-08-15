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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

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
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
		template<typename T>
		T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));;
}