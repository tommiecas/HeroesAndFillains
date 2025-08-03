// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "Components/ProgressBar.h"
#include "HUD/EnemyHealthBarWidget.h"

void UEnemyHealthBarWidgetComponent::SetHealthPercent(float Percent)
{
	if (EnemyHealthBarWidget == nullptr)
	{
		EnemyHealthBarWidget = Cast<UEnemyHealthBarWidget>(GetUserWidgetObject());
	}
	if (EnemyHealthBarWidget && EnemyHealthBarWidget->EnemyHealthProgressBar)
	{
		EnemyHealthBarWidget->EnemyHealthProgressBar->SetPercent(Percent);
	}
}
