// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProgressBarBaseWidget.h"
#include "FillainShieldWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UFillainShieldWidget : public UProgressBarBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
	void ForceWidgetComponentVisibility(UWidget* WidgetComponent);


	UFUNCTION()
	void UpdateShieldBar(float NewShield);

	UFUNCTION()
	void UpdateMaxShieldText(float NewMaxShield);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* ShieldProgressBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ShieldTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MaxShieldTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* ShieldIconImage;

	UPROPERTY()
	float NumberedShield = 100.f;

	UPROPERTY()
	float NumberedMaxShield = 100.f;

	bool bHasBoundDelegates = false;

	FORCEINLINE UProgressBar* GetShieldProgressBar() const { return ShieldProgressBar; }
	FORCEINLINE UTextBlock* GetShieldTextBox() const { return ShieldTextBox; }
	FORCEINLINE UTextBlock* GetMaxShieldTextBox() const { return MaxShieldTextBox; }
	FORCEINLINE UImage* GetShieldIconImage() const { return ShieldIconImage; }
};
