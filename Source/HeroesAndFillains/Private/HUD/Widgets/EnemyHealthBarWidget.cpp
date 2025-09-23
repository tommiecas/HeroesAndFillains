// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widgets/EnemyHealthBarWidget.h"

#include "Enemies/EnemyBase.h"


void UEnemyHealthBarWidget::UpdateOwnerEnemy(class AEnemyBase* NewEnemy)
{
	Super::UpdateOwnerEnemy(NewEnemy);
}

void UEnemyHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
