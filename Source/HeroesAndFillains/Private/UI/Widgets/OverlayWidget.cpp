// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidget.h"

void UOverlayWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
{
	if (WidgetComponent)
	{
		// WidgetComponent->SetVisibility(ESlateVisibility::HitTestInvisible);
		// WidgetComponent->SetRenderOpacity(1.f);
		// WidgetComponent->SetIsEnabled(true);
		// WidgetComponent->SetRenderScale(FVector2D(1.f, 1.f));
		// WidgetComponent->SetRenderTranslation(FVector2D(0.0f, 0.0f));
	}	
}

void UOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ForceWidgetComponentVisibility(this);
}
