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
	virtual void NativeConstruct() override;
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
	void ForceWidgetComponentVisibility(UWidget* WidgetComponent);


	UFUNCTION()
	void UpdateMajixBar(float NewMajix);

	UFUNCTION()
	void UpdateMaxMajixText(float NewMaxMajix);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* MajixProgressBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReaOnlye, meta = (BindWidget))
	class UTextBlock* MajixTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MaxMajixTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* MajixIconImage;

	UPROPERTY()
	float NumberedMajix = 100.f;

	UPROPERTY()
	float NumberedMaxMajix = 100.f;

	bool bHasBoundDelegates = false;

};
