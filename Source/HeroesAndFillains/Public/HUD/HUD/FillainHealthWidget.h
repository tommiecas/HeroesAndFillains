// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProgressBarBaseWidget.h"
#include "FillainHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UFillainHealthWidget : public UProgressBarBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
	void ForceWidgetComponentVisibility(UWidget* WidgetComponent);


	UFUNCTION()
	void UpdateHealthBar(float NewHealth);

	UFUNCTION()
	void UpdateMaxHealthText(float NewMaxHealth);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReaOnlye, meta = (BindWidget))
	class UTextBlock* HealthTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MaxHealthTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* HealthIconImage;

	UPROPERTY()
	float NumberedHealth = 100.f;

	UPROPERTY()
	float NumberedMaxHealth = 100.f;

	bool bHasBoundDelegates = false;

};
