// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainHealthWidget.h"
#include "HUD/CharacterOverlayFixed.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UFillainHealthWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
{
   if (WidgetComponent)
   {
      WidgetComponent->SetVisibility(ESlateVisibility::Visible);
      WidgetComponent->SetRenderOpacity(1.0f);
      WidgetComponent->SetIsEnabled(true);
      WidgetComponent->SetRenderScale(FVector2D(1.f, 1.f));
      WidgetComponent->SetRenderTranslation(FVector2D(0.0f, 0.0f));
   }
}


void UFillainHealthWidget::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::Visible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   if (HealthProgressBar) ForceWidgetComponentVisibility(HealthProgressBar);
   if (HealthTextBox) ForceWidgetComponentVisibility(HealthTextBox);
   if (HealthIconImage) ForceWidgetComponentVisibility(HealthIconImage);
}


void UFillainHealthWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
   if (!InWidgetController) return;


   if (bHasBoundDelegates) return;
   bHasBoundDelegates = true;
   
   OverlayWidgetController = InWidgetController;
   
   // Avoid double-binding each delegate
   if (!InWidgetController->OnHealthChanged.IsAlreadyBound(this, &UFillainHealthWidget::UpdateHealthBar))
      InWidgetController->OnHealthChanged.AddDynamic(this, &UFillainHealthWidget::UpdateHealthBar);


   if (!InWidgetController->OnMaxHealthChanged.IsAlreadyBound(this, &UFillainHealthWidget::UpdateMaxHealthText))
      InWidgetController->OnMaxHealthChanged.AddDynamic(this, &UFillainHealthWidget::UpdateMaxHealthText);
   
}


void UFillainHealthWidget::UpdateHealthBar(float NewHealth)
{
   NumberedHealth = NewHealth;
   if (HealthProgressBar && NumberedMaxHealth > 0.f)
   {
      HealthProgressBar->SetPercent(NumberedHealth / NumberedMaxHealth);
      HealthTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewHealth)));
   }
}


void UFillainHealthWidget::UpdateMaxHealthText(float NewMaxHealth)
{
   NumberedMaxHealth = NewMaxHealth;
   if (MaxHealthTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
         static_cast<int32>(NumberedMaxHealth));
      MaxHealthTextBox->SetText(FText::FromString(ValueString));
   }
}
