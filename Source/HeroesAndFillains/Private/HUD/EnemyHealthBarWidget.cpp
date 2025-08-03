// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EnemyHealthBarWidget.h"

#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::SetEnemyHealthPercent(float HealthPercent)
{
	if (EnemyHealthProgressBar)
	{
		EnemyHealthProgressBar->SetPercent(HealthPercent);
	}
}
