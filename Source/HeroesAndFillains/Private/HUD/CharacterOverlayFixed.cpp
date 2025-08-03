// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlayFixed.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "HUD/FillainHUD.h"


#include "HUD/WidgetControllers/OverlayWidgetController.h"


void UCharacterOverlayFixed::ForceWidgetComponentVisibility(UWidget* WidgetComponent)
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


void UCharacterOverlayFixed::NativeConstruct()
{
   Super::NativeConstruct();

   SetVisibility(ESlateVisibility::Visible);
   SetRenderOpacity(1.0f);
   SetIsEnabled(true);
   SetRenderScale(FVector2D(1.f, 1.f));
   if (HealthProgressBar) ForceWidgetComponentVisibility(HealthProgressBar);
   if (ShieldProgressBar) ForceWidgetComponentVisibility(ShieldProgressBar);
   if (StaminaProgressBar) ForceWidgetComponentVisibility(StaminaProgressBar);
   if (MajixProgressBar) ForceWidgetComponentVisibility(MajixProgressBar);
   if (HealthTextBox) ForceWidgetComponentVisibility(HealthTextBox);
   if (ShieldTextBox) ForceWidgetComponentVisibility(ShieldTextBox);
   if (StaminaTextBox) ForceWidgetComponentVisibility(StaminaTextBox);
   if (MajixTextBox) ForceWidgetComponentVisibility(MajixTextBox);
   if (HealthIconImage) ForceWidgetComponentVisibility(HealthIconImage);
   if (ShieldIconImage) ForceWidgetComponentVisibility(ShieldIconImage);
   if (StaminaIconImage) ForceWidgetComponentVisibility(StaminaIconImage);
   if (MajixIconImage) ForceWidgetComponentVisibility(MajixIconImage);
}


void UCharacterOverlayFixed::SetWidgetController(UOverlayWidgetController* InWidgetController)
{
   if (!InWidgetController) return;


   if (bHasBoundDelegates) return;
   bHasBoundDelegates = true;


   OverlayWidgetController = InWidgetController;


   // Avoid double-binding each delegate
   if (!InWidgetController->OnHealthChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateHealthBar))
      InWidgetController->OnHealthChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateHealthBar);


   if (!InWidgetController->OnMaxHealthChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateMaxHealthText))
      InWidgetController->OnMaxHealthChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateMaxHealthText);


   if (!InWidgetController->OnShieldChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateShieldBar))
      InWidgetController->OnShieldChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateShieldBar);


   if (!InWidgetController->OnMaxShieldChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateMaxShieldText))
      InWidgetController->OnMaxShieldChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateMaxShieldText);


   if (!InWidgetController->OnStaminaChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateStaminaBar))
      InWidgetController->OnStaminaChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateStaminaBar);


   if (!InWidgetController->OnMaxStaminaChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateMaxStaminaText))
      InWidgetController->OnMaxStaminaChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateMaxStaminaText);


   if (!InWidgetController->OnMajixChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateMajixBar))
      InWidgetController->OnMajixChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateMajixBar);


   if (!InWidgetController->OnMaxMajixChanged.IsAlreadyBound(this, &UCharacterOverlayFixed::UpdateMaxMajixText))
      InWidgetController->OnMaxMajixChanged.AddDynamic(this, &UCharacterOverlayFixed::UpdateMaxMajixText);
   
}


void UCharacterOverlayFixed::UpdateHealthBar(float NewHealth)
{
   NumberedHealth = NewHealth;
   if (HealthProgressBar && NumberedMaxHealth > 0.f)
   {
      HealthProgressBar->SetPercent(NumberedHealth / NumberedMaxHealth);
      HealthTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewHealth)));
   }
}


void UCharacterOverlayFixed::UpdateMaxHealthText(float NewMaxHealth)
{
   NumberedMaxHealth = NewMaxHealth;
   if (MaxHealthTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
         static_cast<int32>(NumberedMaxHealth));
      MaxHealthTextBox->SetText(FText::FromString(ValueString));
   }
}
void UCharacterOverlayFixed::UpdateShieldBar(float NewShield)
{
   NumberedShield = NewShield;
   if (ShieldProgressBar && NumberedMaxShield > 0.f)
   {
      ShieldProgressBar->SetPercent(NumberedShield / NumberedMaxShield);
      ShieldTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewShield)));
   }
}


void UCharacterOverlayFixed::UpdateMaxShieldText(float NewMaxShield)
{
   NumberedMaxShield = NewMaxShield;
   if (MaxShieldTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
               static_cast<int32>(NumberedMaxHealth));
      MaxShieldTextBox->SetText(FText::FromString(ValueString));
   }
}


void UCharacterOverlayFixed::UpdateStaminaBar(float NewStamina)
{
   NumberedStamina = NewStamina;
   if (StaminaProgressBar && NumberedMaxStamina > 0.f)
   {
      StaminaProgressBar->SetPercent(NumberedStamina / NumberedMaxStamina);
      StaminaTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewStamina)));
   }
}


void UCharacterOverlayFixed::UpdateMaxStaminaText(float NewMaxStamina)
{
   NumberedMaxStamina = NewMaxStamina;
   if (MaxStaminaTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
               static_cast<int32>(NumberedMaxStamina));
      MaxStaminaTextBox->SetText(FText::FromString(ValueString));
   }
}


void UCharacterOverlayFixed::UpdateMajixBar(float NewMajix)
{
   NumberedMajix = NewMajix;
   if (MajixProgressBar && NumberedMaxMajix > 0.f)
   {
      MajixProgressBar->SetPercent(NumberedMajix / NumberedMaxMajix);
      MajixTextBox->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), NewMajix)));
   }
}


void UCharacterOverlayFixed::UpdateMaxMajixText(float NewMaxMajix)
{
   NumberedMaxMajix = NewMaxMajix;
   if (MaxMajixTextBox)
   {
      const FString ValueString = FString::Printf(TEXT("%d"),
               static_cast<int32>(NumberedMaxMajix));
      MaxMajixTextBox->SetText(FText::FromString(ValueString));
   }
}

