// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/EnemyStatsWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Enemies/EnemyBase.h"
#include "UI/Widgets/EnemyProgressBarBaseWidget.h"

UEnemyStatsWidget::UEnemyStatsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	
}

void UEnemyStatsWidget::InitializeForEnemy(AEnemyBase* Enemy)
{
	{
		if (!Enemy) return;

		if (WBP_EnemyHealthBarWidget)
		{
			WBP_EnemyHealthBarWidget->UpdateOwnerEnemy(Enemy);
		}
		if (WBP_EnemyShieldBarWidget)
		{
			WBP_EnemyShieldBarWidget->UpdateOwnerEnemy(Enemy);
		}
	}
}

void UEnemyStatsWidget::RefreshVisibility(float Health, float Shield)
{
	{
		if (WBP_EnemyShieldBarWidget)
		{
			WBP_EnemyShieldBarWidget->SetVisibility(Shield > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}

		if (WBP_EnemyHealthBarWidget)
		{
			// Health bar is always visible, but shield may cover it
			WBP_EnemyHealthBarWidget->SetVisibility(Health > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}
	}
}

void UEnemyStatsWidget::UpdateStats(float Health, float MaxHealth, float Shield, float MaxShield)
{
	if (WBP_EnemyHealthBarWidget)
	{
		WBP_EnemyHealthBarWidget->SetVisibility(Health > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (WBP_EnemyShieldBarWidget)
	{
		if (Shield > 0)
		{
			WBP_EnemyShieldBarWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			WBP_EnemyShieldBarWidget->RemoveFromParent();
		}
	}
}
 
void UEnemyStatsWidget::InitForEnemy(AEnemyBase* InEnemy)
{
	Super::InitForEnemy(InEnemy);

	if (WBP_EnemyHealthBarWidget)
	{
		WBP_EnemyHealthBarWidget->bInitialized = true;
		WBP_EnemyHealthBarWidget->UpdateOwnerEnemy(InEnemy);  // sets "Base Enemy" in BP
	}

	if (WBP_EnemyShieldBarWidget)
	{
		WBP_EnemyShieldBarWidget->UpdateOwnerEnemy(InEnemy);  // sets "Base Enemy" in BP
	}
}

void UEnemyStatsWidget::UpdateOwnerEnemy(AEnemyBase* NewEnemy)
{
	Super::UpdateOwnerEnemy(NewEnemy);

	if (WBP_EnemyHealthBarWidget)
	{ 
		WBP_EnemyHealthBarWidget->bInitialized = true;
		WBP_EnemyHealthBarWidget->UpdateOwnerEnemy(NewEnemy);
	}

	if (WBP_EnemyShieldBarWidget)
	{
		WBP_EnemyShieldBarWidget->UpdateOwnerEnemy(NewEnemy);
	}
}
