// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainHealthWidget.h"

void UFillainHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShowVitalAttributeWidget(HealthBar, HealthImage, HealthText);
}
