// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD/FillainMajixWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UFillainMajixWidget::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
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


void UFillainMajixWidget::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::Visible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   if (MajixProgressBar) ForceWidgetComponentVisibility(MajixProgressBar);
   if (MajixTextBox) ForceWidgetComponentVisibility(MajixTextBox);
   if (MajixIconImage) ForceWidgetComponentVisibility(MajixIconImage);
}


void UFillainMajixWidget::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
   if (!InWidgetController) return;


   if (bHasBoundDelegates) return;
   bHasBoundDelegates = true;
   
   OverlayWidgetController = InWidgetController;
   
   // Avoid double-binding each delegate
   if (!InWidgetController->OnMajixChanged.IsAlreadyBound(this, &UFillainMajixWidget::UpdateMajixBar))
      InWidgetController->OnMajixChanged.AddDynamic(this, &UFillainMajixWidget::UpdateMajixBar);


   if (!InWidgetController->OnMaxMajixChanged.IsAlreadyBound(this, &UFillainMajixWidget::UpdateMaxMajixText))
      InWidgetController->OnMaxMajixChanged.AddDynamic(this, &UFillainMajixWidget::UpdateMaxMajixText);
   
}


void UFillainMajixWidget::UpdateMajixBar(float NewMajix)
{
   NumberedMajix = NewMajix;
   if (MajixProgressBar && NumberedMaxMajix> 0.f)
   {
      MajixProgressBar->SetPercent(NumberedMajix / NumberedMaxMajix);
      MajixTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewMajix)));
   }
}


void UFillainMajixWidget::UpdateMaxMajixText(float NewMaxMajix)
{
   NumberedMaxMajix = NewMaxMajix;
   if (MaxMajixTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
         static_cast<int32>(NumberedMaxMajix));
      MaxMajixTextBox->SetText(FText::FromString(ValueString));
   }
}
