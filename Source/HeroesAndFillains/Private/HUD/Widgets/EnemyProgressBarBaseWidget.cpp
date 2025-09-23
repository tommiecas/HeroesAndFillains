// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widgets/EnemyProgressBarBaseWidget.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"


void UEnemyProgressBarBaseWidget::UpdateOwnerEnemy(AEnemyBase* NewEnemy)
{
	OwnerEnemy = NewEnemy;
	
}

void UEnemyProgressBarBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// If OwnerEnemy was set earlier (e.g. in PossessedBy), update UI now.
	if (OwnerEnemy)
	{
		AEnemyBase* BaseEnemy = Cast<AEnemyBase>(OwnerEnemy);
		if (BaseEnemy && BaseEnemy->EnemyHealthBar)
		{
			if (UAttributeSet* EnAttSet = BaseEnemy->GetAttributeSet())
			{
				if (UHAFAttributeSet* HAFEnemyAttSet = Cast<UHAFAttributeSet>(EnAttSet))
				{
					float H = HAFEnemyAttSet->GetHealth();
					float MH = HAFEnemyAttSet->GetMaxHealth();
					float Per = H / MH;
				}
			}
		}
	}
}
