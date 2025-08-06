// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainShieldWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UFillainShieldWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
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


void UFillainShieldWidget::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::Visible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   if (ShieldProgressBar) ForceWidgetComponentVisibility(ShieldProgressBar);
   if (ShieldTextBox) ForceWidgetComponentVisibility(ShieldTextBox);
   if (ShieldIconImage) ForceWidgetComponentVisibility(ShieldIconImage);
}


void UFillainShieldWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
   if (!InWidgetController) return;


   if (bHasBoundDelegates) return;
   bHasBoundDelegates = true;
   
   OverlayWidgetController = InWidgetController;
   
   // Avoid double-binding each delegate
   if (!InWidgetController->OnShieldChanged.IsAlreadyBound(this, &UFillainShieldWidget::UpdateShieldBar))
      InWidgetController->OnShieldChanged.AddDynamic(this, &UFillainShieldWidget::UpdateShieldBar);


   if (!InWidgetController->OnMaxShieldChanged.IsAlreadyBound(this, &UFillainShieldWidget::UpdateMaxShieldText))
      InWidgetController->OnMaxShieldChanged.AddDynamic(this, &UFillainShieldWidget::UpdateMaxShieldText);
   
}


void UFillainShieldWidget::UpdateShieldBar(float NewShield)
{
   NumberedShield = NewShield;
   if (ShieldProgressBar && NumberedMaxShield> 0.f)
   {
      ShieldProgressBar->SetPercent(NumberedShield / NumberedMaxShield);
      ShieldTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewShield)));
   }
}


void UFillainShieldWidget::UpdateMaxShieldText(float NewMaxShield)
{
   NumberedMaxShield = NewMaxShield;
   if (MaxShieldTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
         static_cast<int32>(NumberedMaxShield));
      MaxShieldTextBox->SetText(FText::FromString(ValueString));
   }
}
