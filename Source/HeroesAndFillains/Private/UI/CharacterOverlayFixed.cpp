// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterOverlayFixed.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI/FillainHUD.h"


#include "UI/WidgetControllers/OverlayWidgetController.h"


void UCharacterOverlayFixed::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
{
   if (WidgetComponent)
   {
      WidgetComponent->SetVisibility(ESlateVisibility::HitTestInvisible);
      WidgetComponent->SetRenderOpacity(1.0f);
      WidgetComponent->SetIsEnabled(true);
      WidgetComponent->SetRenderScale(FVector2D(1.f, 1.f));
      WidgetComponent->SetRenderTranslation(FVector2D(0.0f, 0.0f));
   }
}


void UCharacterOverlayFixed::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::HitTestInvisible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   
}







