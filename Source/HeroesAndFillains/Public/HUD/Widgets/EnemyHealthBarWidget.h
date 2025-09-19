// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyProgressBarBaseWidget.h"
#include "EnemyHealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyHealthBarWidget : public UEnemyProgressBarBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void OnWidgetConstructed();
};
