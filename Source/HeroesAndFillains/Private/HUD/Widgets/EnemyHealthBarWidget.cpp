// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widgets/EnemyHealthBarWidget.h"

#include "Enemies/EnemyBase.h"

void UEnemyHealthBarWidget::OnWidgetConstructed_Implementation()
{
	AEnemyBase* WidgetControllingEnemyBase = Cast<AEnemyBase>(OwnerEnemy);
}
