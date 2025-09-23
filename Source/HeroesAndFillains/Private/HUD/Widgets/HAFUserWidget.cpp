// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widgets/HAFUserWidget.h"

#include "Components/WidgetComponent.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UHAFUserWidget::SetWidgetController(UObject* InWidgetController)
{
	if (!InWidgetController) return;

	WidgetController = InWidgetController;
	WidgetControllerSet();
}
