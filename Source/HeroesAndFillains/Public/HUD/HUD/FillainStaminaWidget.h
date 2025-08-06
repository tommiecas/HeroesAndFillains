// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProgressBarBaseWidget.h"
#include "FillainStaminaWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UFillainStaminaWidget : public UProgressBarBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
	void ForceWidgetComponentVisibility(UWidget* WidgetComponent);


	UFUNCTION()
	void UpdateStaminaBar(float NewStamina);

	UFUNCTION()
	void UpdateMaxStaminaText(float NewMaxStamina);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReaOnlye, meta = (BindWidget))
	class UTextBlock* StaminaTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MaxStaminaTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* StaminaIconImage;

	UPROPERTY()
	float NumberedStamina = 100.f;

	UPROPERTY()
	float NumberedMaxStamina = 100.f;

	bool bHasBoundDelegates = false;

};
