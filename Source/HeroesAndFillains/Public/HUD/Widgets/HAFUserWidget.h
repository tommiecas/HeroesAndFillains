// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HAFUserWidget.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UObject> OwnerEnemy = nullptr;

	// virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

private:

public:
	FORCEINLINE UObject* GetWidgetController() const { return WidgetController; }
	FORCEINLINE UObject* GetOwnerEnemy() const { return OwnerEnemy; }

	
};