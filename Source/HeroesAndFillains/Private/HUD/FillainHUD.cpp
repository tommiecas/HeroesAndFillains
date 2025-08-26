// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/FillainHUD.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "HUD/CharacterOverlayFixed.h"
#include "HUD/Announcement.h"
#include "HUD/EliminationAnnouncement.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/BaseCharacter.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "HUD/CharacterOverlayFixed.h"
#include "HUD/Widgets/HAFUserWidget.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"
#include "PlayerState/HAFPlayerState.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Chaos/Math/Poisson.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HUD/WidgetControllers/AttributeMenuWidgetController.h"
#include "Widgets/Layout/SScaleBox.h"


void AFillainHUD::AddCharacterOverlayFixed()
{
	return;
}

void AFillainHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AFillainHUD::AddEliminationAnnouncement(FString Killer, FString Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	if (OwningPlayer && EliminationAnnouncementClass)
	{
		UEliminationAnnouncement* EliminationAnnouncementWidget = CreateWidget<UEliminationAnnouncement>(OwningPlayer, EliminationAnnouncementClass);
		if (EliminationAnnouncementWidget)
		{
			EliminationAnnouncementWidget->SetEliminationAnnouncementText(Killer, Victim);
			EliminationAnnouncementWidget->AddToViewport();

			for (UEliminationAnnouncement* Message : EliminationMessages)
			{
				if (Message && Message->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Message->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
						CanvasSlot->SetPosition(NewPosition);
					}

				}
			}

			EliminationMessages.Add(EliminationAnnouncementWidget);
			FTimerHandle EliminationMessageTimer;
			FTimerDelegate EliminationMessageDelegate;
			EliminationMessageDelegate.BindUFunction(this, FName("EliminationAnnouncementTimerFinished"), EliminationAnnouncementWidget);
			GetWorldTimerManager().SetTimer(EliminationMessageTimer, EliminationMessageDelegate, EliminationAnnouncementTime, false);
		}
	}
}

UOverlayWidgetController* AFillainHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AFillainHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

void AFillainHUD::InitializeOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class not set on BP_FillainHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class not set on BP_FillainHUD"));
	checkf(CharacterOverlayWidgetFixedClass, TEXT("Character Overlay Widget Fixed Class not set on BP_FillainHUD"));

	// Build controller and bind to GAS (once)
	if (!OverlayWidgetController)
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwningPawn());
		const FWidgetControllerParams Params(GetOwningPlayerController(), GetOwningPlayerController()->GetPlayerState<AHAFPlayerState>(), Character->GetAbilitySystemComponent(), Character->GetAttributeSet());
		OverlayWidgetController = GetOverlayWidgetController(Params);
		OverlayWidgetController->BindCallbacksToDependencies();
	}

	// Create fixed overlay (with owning player) if you need it
	if (!CharacterOverlayWidgetFixed)
	{
		CharacterOverlayWidgetFixed =
			CreateWidget<UCharacterOverlayFixed>(GetOwningPlayerController(), CharacterOverlayWidgetFixedClass);
		if (CharacterOverlayWidgetFixed)
		{
			CharacterOverlayWidgetFixed->AddToViewport(100);
			CharacterOverlayWidgetFixed->SetVisibility(ESlateVisibility::SelfHitTestInvisible); // cannot eat clicks
		}
	}

	// Create the main overlay (with owning player)
	if (!OverlayWidget)
	{
		OverlayWidget = Cast<UHAFUserWidget>(CreateWidget<UUserWidget>(GetOwningPlayerController(), OverlayWidgetClass));
		if (!OverlayWidget) return; // wrong class assigned in BP?
	}

	// Let the widget bind to controller delegates (your BP runs here)
	OverlayWidget->SetWidgetController(OverlayWidgetController);
	
	// Push initial values (now the widget will receive them)
	OverlayWidgetController->BroadcastInitialValues();

	// Finally show it
	if (!OverlayWidget->IsInViewport())
	{
		OverlayWidget->AddToViewport(200);
		OverlayWidget->SetVisibility(ESlateVisibility::Visible);   // MUST be Visible, not SelfHitTestInvisible
		OverlayWidget->SetIsEnabled(true);
	}
	if (AFillainCharacter* Fillian = Cast<AFillainCharacter>(GetOwningPawn()))
	{
		Fillian->LogSecondaries_Client();
		UAbilitySystemComponent* ASComp = Cast<UAbilitySystemComponent>(Fillian->GetAbilitySystemComponent());
		UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(Fillian->GetAttributeSet());
		if (Fillian->IsLocallyControlled() && ASComp && HAFAttributeSet)
		{
			AFillainCharacter* FillainChar = Cast<AFillainCharacter>(GetOwningPawn());
			UE_LOG(LogTemp, Warning, TEXT("[CLIENT] On Widget Init: MaxHealth=%.3f Armor=%.3f Crit=%.3f"),
				FillainChar->SafeGet(ASComp, HAFAttributeSet, HAFAttributeSet->GetMaxHealthAttribute()),
				FillainChar->SafeGet(ASComp, HAFAttributeSet, HAFAttributeSet->GetArmorAttribute()),
				FillainChar->SafeGet(ASComp, HAFAttributeSet, HAFAttributeSet->GetCriticalHitChanceAttribute()));
		}
	}
}

void AFillainHUD::EliminationAnnouncementTimerFinished(UEliminationAnnouncement* MessageToRemove)
{
	if (MessageToRemove)
	{
		MessageToRemove->RemoveFromParent();
	}
}

void AFillainHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		/* if (HUDPackage.CrosshairsColor == FLinearColor::Red)
		{
			SpreadScaled = SpreadScaled * .8f;
		}*/

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread/*, HUDPackage.CrosshairsColor */ );
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread/*, HUDPackage.CrosshairsColor*/);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread/*, HUDPackage.CrosshairsColor*/);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread/*, HUDPackage.CrosshairsColor*/);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread/*, HUDPackage.CrosshairsColor*/);
		}
	}
}

void AFillainHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread/*, FLinearColor CrosshairsColor*/)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X, ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
		// CrosshairsColor
	);


}
