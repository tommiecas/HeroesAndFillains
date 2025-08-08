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
	
    
    bool bHasBoundDelegates = false;

	
	
	
	
	
	
	
	
	
};