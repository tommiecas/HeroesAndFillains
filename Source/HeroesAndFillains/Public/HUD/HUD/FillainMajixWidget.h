// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProgressBarBaseWidget.h"
#include "FillainMajixWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UFillainMajixWidget : public UProgressBarBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* MajixBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* MajixImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MajixText;
	
protected:
	virtual void NativeConstruct() override;
};
