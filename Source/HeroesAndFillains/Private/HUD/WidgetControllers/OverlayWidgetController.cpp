// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WidgetControllers/OverlayWidgetController.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(HAFAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(HAFAttributeSet->GetMaxHealth());
	OnShieldChanged.Broadcast(HAFAttributeSet->GetShield());
	OnMaxShieldChanged.Broadcast(HAFAttributeSet->GetMaxShield());
	OnStaminaChanged.Broadcast(HAFAttributeSet->GetStamina());
	OnMaxStaminaChanged.Broadcast(HAFAttributeSet->GetMaxStamina());
	OnMajixChanged.Broadcast(HAFAttributeSet->GetMajix());
	OnMaxMajixChanged.Broadcast(HAFAttributeSet->GetMaxMajix());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	HAFAttributeSet->GetShieldAttribute()).AddUObject(this, &UOverlayWidgetController::ShieldChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxShieldAttribute()).AddUObject(this, &UOverlayWidgetController::MaxShieldChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	HAFAttributeSet->GetStaminaAttribute()).AddUObject(this, &UOverlayWidgetController::StaminaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxStaminaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxStaminaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	HAFAttributeSet->GetMajixAttribute()).AddUObject(this, &UOverlayWidgetController::MajixChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxMajixAttribute()).AddUObject(this, &UOverlayWidgetController::MaxMajixChanged);

	Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this] (const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}	
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnShieldChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxShieldChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::StaminaChanged(const FOnAttributeChangeData& Data) const
{
	OnStaminaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxStaminaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxStaminaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MajixChanged(const FOnAttributeChangeData& Data) const
{
	OnMajixChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxMajixChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxMajixChanged.Broadcast(Data.NewValue);
}
			
