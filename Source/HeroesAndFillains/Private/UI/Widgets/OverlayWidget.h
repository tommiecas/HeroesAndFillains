// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/HAFUserWidget.h"
#include "OverlayWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UOverlayWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreSpacerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesText;

	UPROPERTY(meta = (BindWidget))
	UImage* HighPingImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EliminationMessageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillerNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VictimNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponTypeText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* EliminationAnimation = nullptr;

	virtual void NativeConstruct() override;
	static void ForceWidgetComponentVisibility(UWidget* WidgetComponent);

	FORCEINLINE UWidgetAnimation* GetEliminationAnimation() const { return EliminationAnimation; }



	

	
};
