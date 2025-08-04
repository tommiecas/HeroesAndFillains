// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFUserWidget.h"
#include "ProgressBarBaseWidget.generated.h"

class UOverlayWidgetController;
class UTextBlock;
class UImage;
class UProgressBar;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API UProgressBarBaseWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	UProgressBarBaseWidget();

	UFUNCTION(BlueprintCallable)
	virtual void SetProgressBarPercent(float Percent, UProgressBar* ProgressBar);
	
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
	
protected:
	virtual void NativeConstruct() override;

	

};