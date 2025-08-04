// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/ProgressBarBaseWidget.h"

#include "FindInBlueprints.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"

UProgressBarBaseWidget::UProgressBarBaseWidget()
{
	
}

void UProgressBarBaseWidget::SetProgressBarPercent(float Percent, UProgressBar* ProgressBar)
{
	if (ProgressBar)
	{
		ProgressBar->SetPercent(Percent);
	}
}

void UProgressBarBaseWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
	Super::SetWidgetController(InWidgetController);
}


void UProgressBarBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}