// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainStaminaWidget.h"

void UFillainStaminaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShowVitalAttributeWidget(StaminaBar, StaminaImage, StaminaText);
}
