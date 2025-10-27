// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/EnemyBaseWidget.h"
#include "EnemyProgressBarBaseWidget.generated.h"

class AEnemyBase;
class UEnemyHealthBarWidget;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyProgressBarBaseWidget : public UEnemyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UWidgetComponent> EnemyProgressBarBaseWidgetOwningComponent;

	UFUNCTION() 
	UWidgetComponent* GetEnemyProgressBarBaseWidgetOwningComponent() const { return EnemyProgressBarBaseWidgetOwningComponent.Get(); }

	virtual void UpdateOwnerEnemy(class AEnemyBase* NewEnemy) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	bool bInitialized = false;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;


		
	
};
