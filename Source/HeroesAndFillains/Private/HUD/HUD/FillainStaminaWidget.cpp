// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainStaminaWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UFillainStaminaWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
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


void UFillainStaminaWidget::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::Visible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   if (StaminaProgressBar) ForceWidgetComponentVisibility(StaminaProgressBar);
   if (StaminaTextBox) ForceWidgetComponentVisibility(StaminaTextBox);
   if (StaminaIconImage) ForceWidgetComponentVisibility(StaminaIconImage);
}


void UFillainStaminaWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
   if (!InWidgetController) return;


   if (bHasBoundDelegates) return;
   bHasBoundDelegates = true;
   
   OverlayWidgetController = InWidgetController;
   
   // Avoid double-binding each delegate
   if (!InWidgetController->OnStaminaChanged.IsAlreadyBound(this, &UFillainStaminaWidget::UpdateStaminaBar))
      InWidgetController->OnStaminaChanged.AddDynamic(this, &UFillainStaminaWidget::UpdateStaminaBar);


   if (!InWidgetController->OnMaxStaminaChanged.IsAlreadyBound(this, &UFillainStaminaWidget::UpdateMaxStaminaText))
      InWidgetController->OnMaxStaminaChanged.AddDynamic(this, &UFillainStaminaWidget::UpdateMaxStaminaText);
   
}


void UFillainStaminaWidget::UpdateStaminaBar(float NewStamina)
{
   NumberedStamina = NewStamina;
   if (StaminaProgressBar && NumberedMaxStamina> 0.f)
   {
      StaminaProgressBar->SetPercent(NumberedStamina / NumberedMaxStamina);
      StaminaTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewStamina)));
   }
}


void UFillainStaminaWidget::UpdateMaxStaminaText(float NewMaxStamina)
{
   NumberedMaxStamina = NewMaxStamina;
   if (MaxStaminaTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
         static_cast<int32>(NumberedMaxStamina));
      MaxStaminaTextBox->SetText(FText::FromString(ValueString));
   }
}
