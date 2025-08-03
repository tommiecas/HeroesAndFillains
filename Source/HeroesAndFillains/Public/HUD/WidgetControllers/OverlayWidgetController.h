// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/WidgetControllers/HAFWidgetController.h"
#include "GameplayEffectTypes.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChangedSignature, float, NewShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxShieldChangedSignature, float, NewMaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChangedSignature, float, NewMaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMajixChangedSignature, float, NewMajix);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxMajixChangedSignature, float, NewMaxMajix);

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

protected:
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ShieldChanged(const FOnAttributeChangeData& Data) const;
	void MaxShieldChanged(const FOnAttributeChangeData& Data) const;
	void StaminaChanged(const FOnAttributeChangeData& Data) const;
	void MaxStaminaChanged(const FOnAttributeChangeData& Data) const;
	void MajixChanged(const FOnAttributeChangeData& Data) const;
	void MaxMajixChanged(const FOnAttributeChangeData& Data) const;
	
};
