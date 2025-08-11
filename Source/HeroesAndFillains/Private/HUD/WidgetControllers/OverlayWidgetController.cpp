// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WidgetControllers/OverlayWidgetController.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(HAFAttributeSet->GetHealth());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial Health: %.2f"), HAFAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(HAFAttributeSet->GetMaxHealth());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial MaxHealth: %.2f"), HAFAttributeSet->GetMaxHealth());
	OnShieldChanged.Broadcast(HAFAttributeSet->GetShield());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial Shield: %.2f"), HAFAttributeSet->GetShield());
	OnMaxShieldChanged.Broadcast(HAFAttributeSet->GetMaxShield());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial MaxShield: %.2f"), HAFAttributeSet->GetMaxShield());
	OnStaminaChanged.Broadcast(HAFAttributeSet->GetStamina());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial Stamina: %.2f"), HAFAttributeSet->GetStamina());
	OnMaxStaminaChanged.Broadcast(HAFAttributeSet->GetMaxStamina());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial MaxStamina: %.2f"), HAFAttributeSet->GetMaxStamina());
	OnMajixChanged.Broadcast(HAFAttributeSet->GetMajix());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial Majix: %.2f"), HAFAttributeSet->GetMajix());
	OnMaxMajixChanged.Broadcast(HAFAttributeSet->GetMaxMajix());
	UE_LOG(LogTemp, Warning, TEXT("📡 Broadcasting Initial MaxMajix: %.2f"), HAFAttributeSet->GetMaxMajix());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent || !AttributeSet) return;

	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetShieldAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
			{
				OnShieldChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetMaxShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxShieldChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetStaminaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
			{
				OnStaminaChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetMaxStaminaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxStaminaChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetMajixAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
			{
				OnMajixChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetMaxMajixAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxMajixChanged.Broadcast(Data.NewValue);
			}
		);


	Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this] (const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// For example, say that Tag = Message.HealthPotion
				// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
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
			
