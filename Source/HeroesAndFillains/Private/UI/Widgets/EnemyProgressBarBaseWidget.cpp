// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/EnemyProgressBarBaseWidget.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Enemies/EnemyBase.h"

void UEnemyProgressBarBaseWidget::UpdateOwnerEnemy(AEnemyBase* NewEnemy)
{
	Super::UpdateOwnerEnemy(NewEnemy);
	OwnerEnemy = NewEnemy;
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