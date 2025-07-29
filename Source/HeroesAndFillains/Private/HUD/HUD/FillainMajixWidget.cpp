// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainMajixWidget.h"

void UFillainMajixWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShowVitalAttributeWidget(MajixBar, MajixImage, MajixText);
}
