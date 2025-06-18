// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarWidgetComponent.h"
#include "Components/ProgressBar.h"
#include "HUD/HealthBarWidget.h"

void UHealthBarWidgetComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}
	if (HealthBarWidget && HealthBarWidget->HealthProgressBar)
	{
		HealthBarWidget->HealthProgressBar->SetPercent(Percent);
	}
}
