// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/ProgressBarBaseWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UProgressBarBaseWidget::UProgressBarBaseWidget()
{
	
}

void UProgressBarBaseWidget::ShowVitalAttributeWidget(UProgressBar* ProgressBar, UImage* Image, UTextBlock* Text)
{
	if (ProgressBar && Image && Text)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
		Image->SetVisibility(ESlateVisibility::Visible);
		Text->SetVisibility(ESlateVisibility::Visible);

		ProgressBar->SetRenderOpacity(1.0f);
		Image->SetOpacity(1.0f);
		Text->SetOpacity(1.0f);
	}
}

void UProgressBarBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

