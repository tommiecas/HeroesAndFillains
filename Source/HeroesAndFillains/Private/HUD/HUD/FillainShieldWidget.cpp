// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainShieldWidget.h"

void UFillainShieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShowVitalAttributeWidget(ShieldBar, ShieldImage, ShieldText);
}
