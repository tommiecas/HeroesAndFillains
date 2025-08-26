// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"

#include "HUD/FillainHUD.h"
#include "HUD/WidgetControllers/HAFWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/HAFPlayerState.h"

UOverlayWidgetController* UHAFAbilitySystemBlueprintLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFillainHUD* FillainHUD = Cast<AFillainHUD>(PC->GetHUD()))
		{
			AHAFPlayerState* PS = PC->GetPlayerState<AHAFPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, AbilitySystemComponent, AS);
			return FillainHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UHAFAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFillainHUD* FillainHUD = Cast<AFillainHUD>(PC->GetHUD()))
		{
			AHAFPlayerState* PS = PC->GetPlayerState<AHAFPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, AbilitySystemComponent, AS);
			return FillainHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
