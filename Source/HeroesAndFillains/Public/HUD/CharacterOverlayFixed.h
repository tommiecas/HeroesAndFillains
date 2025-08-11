// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/HAFUserWidget.h"
#include "CharacterOverlayFixed.generated.h"

UCLASS()
class HEROESANDFILLAINS_API UCharacterOverlayFixed : public UHAFUserWidget
{
	GENERATED_BODY()
public:
   virtual void NativeConstruct() override;
   virtual void SetWidgetController(UOverlayWidgetController* InWidgetController) override;
   void ForceWidgetComponentVisibility(UWidget* WidgetComponent);


   UFUNCTION()
   void UpdateHealthBar(float NewHealth);


   UFUNCTION()
   void UpdateShieldBar(float NewShield);


   UFUNCTION()
   void UpdateStaminaBar(float NewStamina);


   UFUNCTION()
   void UpdateMajixBar(float NewMajix);


   UFUNCTION()
   void UpdateMaxHealthText(float NewMaxHealth);


   UFUNCTION()
   void UpdateMaxShieldText(float NewMaxShield);


   UFUNCTION()
   void UpdateMaxStaminaText(float NewMaxStamina);
  
   UFUNCTION()
   void UpdateMaxMajixText(float NewMaxMajix);


  
   // Health

    // Repeat for Shield
   
	
    // Repeat for Stamina
  
	// Repeat for Majix

  
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreAmount;
	
	UPROPERTY(meta = (BindWidget))
    UTextBlock* RedTeamScore;
	
	UPROPERTY(meta = (BindWidget))
    UTextBlock* BlueTeamScore;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreSpacerText;
  
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DefeatsAmount;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WeaponAmmoAmount;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CarriedAmmoAmount;
  
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WeaponTypeText;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EliminationMessageText;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* VictimNameText;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillerNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MatchCountdownText;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* GrenadesText;
	
    UPROPERTY(meta = (BindWidget))
    class UImage* HighPingImage;
	
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    class UWidgetAnimation* HighPingAnimation;
	
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* EliminationAnimation;
  
    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldCountText;
	
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SoulsCountText;
	
    UPROPERTY()
    UOverlayWidgetController* OverlayWidgetController;
	
    UPROPERTY()
    float NumberedHealth = 100.f;
	
    UPROPERTY()
    float NumberedShield = 100.f;
	
    UPROPERTY()
    float NumberedStamina = 100.f;
	
    UPROPERTY()
    float NumberedMajix = 100.f;
	
    UPROPERTY()
    float NumberedMaxHealth = 100.f;
	
    UPROPERTY()
    float NumberedMaxShield = 100.f;

    UPROPERTY()
    float NumberedMaxStamina = 100.f;
	
    UPROPERTY()
    float NumberedMaxMajix = 100.f;
	
    bool bHasBoundDelegates = false;

	FORCEINLINE UProgressBar* GetHealthProgressBar() const { return HealthProgressBar; }
	FORCEINLINE UProgressBar* GetShieldProgressBar() const { return ShieldProgressBar; }
	FORCEINLINE UProgressBar* GetStaminaProgressBar() const { return StaminaProgressBar; }
	FORCEINLINE UProgressBar* GetMajixProgressBar() const { return MajixProgressBar; }
	FORCEINLINE UTextBlock* GetHealthTextBox() const { return HealthTextBox; }
	FORCEINLINE UTextBlock* GetShieldTextBox() const { return ShieldTextBox; }
	FORCEINLINE UTextBlock* GetStaminaTextBox() const { return StaminaTextBox; }
	FORCEINLINE UTextBlock* GetMajixTextBox() const { return MajixTextBox; }
	FORCEINLINE UTextBlock* GetMaxHealthTextBox() const { return MaxHealthTextBox; }
	FORCEINLINE UTextBlock* GetMaxShieldTextBox() const { return MaxShieldTextBox; }
	FORCEINLINE UTextBlock* GetMaxStaminaTextBox() const { return MaxStaminaTextBox; }
	FORCEINLINE UTextBlock* GetMaxMajixTextBox() const { return MaxMajixTextBox; }
	FORCEINLINE UImage* GetHealthIconImage() const { return HealthIconImage; }
	FORCEINLINE UImage* GetShieldIconImage() const { return ShieldIconImage; }
	FORCEINLINE UImage* GetStaminaIconImage() const { return StaminaIconImage; }
	
};