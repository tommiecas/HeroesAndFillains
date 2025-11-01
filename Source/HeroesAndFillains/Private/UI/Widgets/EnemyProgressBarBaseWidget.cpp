// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/EnemyProgressBarBaseWidget.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Enemies/EnemyBase.h"

void UEnemyProgressBarBaseWidget::InitializeForEnemy(AEnemyBase* Enemy)
{
	if (Enemy && Enemy->HealthBarWidget)
	{
		UpdateOwnerEnemy(Enemy);
	}
	if (Enemy && Enemy->ShieldBarWidget)
	{
		UpdateOwnerEnemy(Enemy);
	}
}

void UEnemyProgressBarBaseWidget::RefreshVisibility(float Health, float Shield, AEnemyBase* Enemy)
{
	if (Enemy && Enemy->ShieldBarWidget)
	{
		Enemy->GetShieldBarWidget()->SetVisibility(Shield > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (Enemy && Enemy->HealthBarWidget)
	{
		// Health bar is always visible, but shield may cover it
		Enemy->GetHealthBarWidget()->SetVisibility(Health > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UEnemyProgressBarBaseWidget::UpdateStats(float Health, float MaxHealth, float Shield, float MaxShield, AEnemyBase* Enemy)
{
	if (Enemy && Enemy->GetHealthBarWidget())
	{
		Enemy->GetHealthBarWidget()->SetVisibility(Health > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (Enemy && Enemy->GetShieldBarWidget())
	{
		if (Shield > 0)
		{
			Enemy->GetShieldBarWidget()->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Enemy->GetShieldBarWidget()->RemoveFromParent();
		}
	}
}

void UEnemyProgressBarBaseWidget::UpdateProgressBar_Implementation(float CurrentValue, float MaxValue)
{
}

void UEnemyProgressBarBaseWidget::InitForEnemy(AEnemyBase* InEnemy)
{
	Super::InitForEnemy(InEnemy);

	if (InEnemy && InEnemy->GetHealthBarWidget())
	{
		InEnemy->GetHealthBarWidget()->bInitialized = true;
		InEnemy->GetHealthBarWidget()->UpdateOwnerEnemy(InEnemy);  // sets "Base Enemy" in BP
	}

	if (InEnemy && InEnemy->GetShieldBarWidget())
	{
		InEnemy->GetShieldBarWidget()->UpdateOwnerEnemy(InEnemy);  // sets "Base Enemy" in BP
	}
}

void UEnemyProgressBarBaseWidget::UpdateOwnerEnemy(AEnemyBase* NewEnemy)
{
	Super::UpdateOwnerEnemy(NewEnemy);

	if (NewEnemy && NewEnemy->GetHealthBarWidget())
	{ 
		NewEnemy->GetHealthBarWidget()->bInitialized = true;
		NewEnemy->GetHealthBarWidget()->UpdateOwnerEnemy(NewEnemy);
	}

	if (NewEnemy && NewEnemy->GetShieldBarWidget())
	{
		NewEnemy->GetShieldBarWidget()->bInitialized = true;
		NewEnemy->GetShieldBarWidget()->UpdateOwnerEnemy(NewEnemy);
	}
}

void UEnemyProgressBarBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Only initialize simple state here.
	bWidgetWorks = false;
}

void UEnemyProgressBarBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Now the UWidgetComponent and widget instance are fully valid.
	if (EnemyProgressBarBaseWidgetOwningComponent)
	{
		if (UUserWidget* WidgetObject = EnemyProgressBarBaseWidgetOwningComponent->GetUserWidgetObject())
		{
			if (WidgetObject == this)
			{
				bWidgetWorks = true;
			}
		}
	}

	// Initialize with current health if OwnerEnemy exists.
	if (OwnerEnemy)
	{
		if (const UAttributeSet* EnAttSet = OwnerEnemy->GetAttributeSet())
		{
			if (const UHAFAttributeSet* HAFEnemyAttSet = Cast<UHAFAttributeSet>(EnAttSet))
			{
				const float Health = HAFEnemyAttSet->GetHealth();
				const float MaxHealth = HAFEnemyAttSet->GetMaxHealth();
				const float Percent = (MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f;

				// Example for when you bind the progress bar:
				// if (HealthBar) HealthBar->SetPercent(Percent);
			}
		}
	}
}