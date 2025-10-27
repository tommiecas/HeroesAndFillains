#pragma once

#include "CoreMinimal.h"
#include "Delegates/AttributeDelegates.h" // ✅ Must come before generated.h
#include "HAFWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

// Forward declarations — remove heavy includes!
class UFillainAttributeMenuWidget;
class AEnemyBase;
struct FGameplayTag;
struct FGameplayAttribute;
class UAttributeInfo;

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API UAttributeMenuWidgetController : public UHAFWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnMaxShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnMajixChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChanged OnMaxMajixChanged;
	
	UPROPERTY(EditDefaultsOnly) 
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
	