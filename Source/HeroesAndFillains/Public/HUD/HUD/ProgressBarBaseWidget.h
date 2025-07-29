// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFUserWidget.h"
#include "ProgressBarBaseWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UProgressBarBaseWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	UProgressBarBaseWidget();
	void ShowVitalAttributeWidget(UProgressBar* ProgressBar, UImage* Image, UTextBlock* TextBlock);
	
protected:
	virtual void NativeConstruct() override;

	

};
