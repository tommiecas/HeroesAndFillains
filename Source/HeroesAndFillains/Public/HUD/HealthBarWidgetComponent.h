// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

	
public:
	void SetHealthPercent(float Percent);

private:
	UPROPERTY()
	class UHealthBarWidget* HealthBarWidget;
	
};
