// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidget.h"

#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"

void UOverlayWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
{
	FString HAFLoadMenu = FString("HAFLoadMenu");
	FString HAFMainMenu = FString("HAFMainMenu");
	if(HAFLoadMenu == (UHAFAbilitySystemBlueprintLibrary::GetCurrentMapName(WidgetComponent)) ||
			HAFMainMenu == (UHAFAbilitySystemBlueprintLibrary::GetCurrentMapName(WidgetComponent)))
	{
		if (WidgetComponent)
		{
			WidgetComponent->SetVisibility(ESlateVisibility::Collapsed);
			WidgetComponent->SetRenderOpacity(0.f);
			WidgetComponent->SetIsEnabled(false);
		}
		else
		{
			WidgetComponent->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			WidgetComponent->SetRenderOpacity(1.f);
			WidgetComponent->SetIsEnabled(true);
			WidgetComponent->SetRenderScale(FVector2D(1.f, 1.f));
			WidgetComponent->SetRenderTranslation(FVector2D(0.0f, 0.0f));
		}
	}	
}

void UOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ForceWidgetComponentVisibility(this);
}
