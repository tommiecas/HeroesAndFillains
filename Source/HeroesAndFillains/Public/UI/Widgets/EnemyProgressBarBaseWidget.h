// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/EnemyBaseWidget.h"
#include "EnemyProgressBarBaseWidget.generated.h"

class AEnemyBase;
class UEnemyHealthBarWidget;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyProgressBarBaseWidget : public UEnemyBaseWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateOwnerEnemy(AEnemyBase* NewEnemy) override;

	UFUNCTION(BlueprintNativeEvent, Category="UI")
	void UpdateProgressBar(float CurrentValue, float MaxValue);

	UFUNCTION(BlueprintCallable, Category="UI")
	void InitializeForEnemy(AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable, Category="UI")
	void RefreshVisibility(float Health, float Shield, AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable, Category="UI")
	void UpdateStats(float Health, float MaxHealth, float Shield, float MaxShield, AEnemyBase* Enemy);

	virtual void InitForEnemy(AEnemyBase* InEnemy) override;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> EnemyProgressBarBaseWidgetOwningComponent;

	UFUNCTION() 
	UWidgetComponent* GetEnemyProgressBarBaseWidgetOwningComponent() const { return EnemyProgressBarBaseWidgetOwningComponent.Get(); }


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	bool bInitialized = false;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;


		
	
};
