// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/EnemyBaseWidget.h"
#include "EnemyStatsWidget.generated.h"

class AEnemyBase;
class UEnemyProgressBarBaseWidget;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyStatsWidget : public UEnemyBaseWidget
{
	GENERATED_BODY()

public:
	explicit UEnemyStatsWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY()
	TObjectPtr<UWidgetComponent> EnemyStatsOwningComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UEnemyProgressBarBaseWidget* WBP_EnemyHealthBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UEnemyProgressBarBaseWidget* WBP_EnemyShieldBarWidget;

	UFUNCTION(BlueprintCallable)
	void InitializeForEnemy(AEnemyBase* Enemy);

	virtual void InitForEnemy(AEnemyBase* InEnemy) override;


	/** Optional: helper to toggle shield/health overlay */
	void RefreshVisibility(float Health, float Shield);

	UFUNCTION(BlueprintCallable)
	UWidgetComponent* GetEnemyStatsOwningComponent() const { return EnemyStatsOwningComponent.Get(); }
	
	void UpdateStats(float Health, float MaxHealth, float Shield, float MaxShield);

	virtual void UpdateOwnerEnemy(AEnemyBase* NewEnemy) override;

};
