// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/HAFUserWidget.h"

void UHAFUserWidget::SetWidgetController(UObject* InWidgetControlller)
{
	WidgetController = InWidgetControlller;
	WidgetControllerSet();
}
