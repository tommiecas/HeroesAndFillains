// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HAFUserWidget.generated.h"

class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(UOverlayWidgetController* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	UOverlayWidgetController* WidgetController;

	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

private:

public:

	
};
