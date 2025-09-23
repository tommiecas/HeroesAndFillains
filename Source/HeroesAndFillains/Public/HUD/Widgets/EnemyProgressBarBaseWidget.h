// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFUserWidget.h"
#include "EnemyProgressBarBaseWidget.generated.h"

class UEnemyHealthBarWidget;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyProgressBarBaseWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void UpdateOwnerEnemy(class AEnemyBase* NewEnemy);

	UPROPERTY()
	UEnemyProgressBarBaseWidget* HealthBarInstance = nullptr;
	
protected:
	virtual void NativeConstruct() override;

		
	
};
