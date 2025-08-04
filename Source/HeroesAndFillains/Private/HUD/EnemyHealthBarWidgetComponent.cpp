// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "Components/ProgressBar.h"
#include "HUD/EnemyHealthBarWidget.h"

void UEnemyHealthBarWidgetComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UEnemyHealthBarWidget>(GetUserWidgetObject());
	}
	if (HealthBarWidget && HealthBarWidget->HealthProgressBar)
	{
		HealthBarWidget->HealthProgressBar->SetPercent(Percent);
	}
}
