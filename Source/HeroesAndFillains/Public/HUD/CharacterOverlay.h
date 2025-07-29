// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/HAFUserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UCharacterOverlay : public UHAFUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UFillainHealthWidget* WBP_FillainHealthWidget;

	UPROPERTY(meta = (BindWidget))
	class UFillainShieldWidget* WBP_FillainShieldWidget;

	UPROPERTY(meta = (BindWidget))
	class UFillainStaminaWidget* WBP_FillainStaminaWidget;

	UPROPERTY(meta = (BindWidget))
	class UFillainMajixWidget* WBP_FillainMajixWidget;

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

	

};
