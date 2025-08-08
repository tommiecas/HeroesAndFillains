// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/HAFUserWidget.h"
#include "OverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UOverlayWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UFillainHealthWidget* FillainHealthWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UFillainShieldWidget* FillainShieldWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UFillainStaminaWidget* FillainStaminaWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  meta = (BindWidget))
	class UFillainMajixWidget* FillainMajixWidget;
};
