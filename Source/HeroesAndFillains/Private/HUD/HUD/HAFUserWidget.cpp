// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/HAFUserWidget.h"

#include "HUD/WidgetControllers/OverlayWidgetController.h"

void UHAFUserWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
	if (!InWidgetController) return;

	WidgetController = InWidgetController;
	WidgetControllerSet();
}

void UHAFUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("✅ UHAFUserWidget::NativeConstruct called on %s"), *GetName());

	if (WidgetController) // Replace with your actual property
	{
		SetWidgetController(WidgetController);
	}
}

