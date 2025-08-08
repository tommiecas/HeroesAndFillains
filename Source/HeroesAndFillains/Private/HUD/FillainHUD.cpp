// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/FillainHUD.h"
#include "GameFramework/PlayerController.h"
#include "HUD/CharacterOverlayFixed.h"
#include "HUD/Announcement.h"
#include "HUD/EliminationAnnouncement.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Characters/FillainCharacter.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "HAFComponents/AttributeComponent.h"
#include "HUD/CharacterOverlayFixed.h"
#include "HUD/OverlayWidget.h"
#include "HUD/HUD/HAFUserWidget.h"
#include "HUD/WidgetControllers/OverlayWidgetController.h"
#include "HUD/HUD/FillainHealthWIdget.h"
#include "HUD/HUD/FillainShieldWidget.h"
#include "HUD/HUD/FillainStaminaWidget.h"
#include "HUD/HUD/FillainMajixWidget.h"

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
		OverlayWidgetController->BindCallbacksToDependencies(); // ✅ Make sure this is called here
	}
	return OverlayWidgetController;
}

void AFillainHUD::InitializeOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC,
	UAttributeSet* AS)
{
	AFillainHUD* HUD = Cast<AFillainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (bIsOverlayInitialized) return; // ✅ Prevent duplicate construction
	if (!OverlayWidget && OverlayWidgetClass)
	{
		OverlayWidget = CreateWidget<UOverlayWidget>(GetWorld(), OverlayWidgetClass);
		if (OverlayWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("✅ Creating Overlay Widget: %s"), *GetName());
			OverlayWidget->AddToViewport(100);
			OverlayWidget->SetVisibility(ESlateVisibility::Visible);
			bIsOverlayInitialized = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to create OverlayWidget from class %s"), *GetNameSafe(OverlayWidgetClass));
		}
	}
	
	ensureMsgf(OverlayWidgetClass, TEXT("OverlayWidgetClass not initialized — check BP_FillainHUD default values!"));
	ensureMsgf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class not initialized. Please fill out  BP_FillainHUD."));

	if (HUD &&
		HUD->OverlayWidget &&
		HUD->OverlayWidget->FillainHealthWidget &&
		HUD->OverlayWidget->FillainShieldWidget &&
		HUD->OverlayWidget->FillainStaminaWidget &&
		HUD->OverlayWidget->FillainMajixWidget)
	{
		AFillainCharacter* Fillain = Cast<AFillainCharacter>(GetOwner());
		UFillainHealthWidget* WBPFillainHealthWidget = HUD->OverlayWidget->FillainHealthWidget;
		WBPFillainHealthWidget->UpdateHealthBar(Fillain->AttributeComponent->GetMaxHealth());
		UFillainShieldWidget* WBPFillainShieldWidget = HUD->OverlayWidget->FillainShieldWidget;
		WBPFillainShieldWidget->UpdateShieldBar(Fillain->AttributeComponent->GetMaxShield());
		UFillainStaminaWidget* WBPFillainStaminaWidget = HUD->OverlayWidget->FillainStaminaWidget;
		WBPFillainStaminaWidget->UpdateStaminaBar(Fillain->AttributeComponent->GetMaxStamina());
		UFillainMajixWidget* WBPFillainMajixWidget = HUD->OverlayWidget->FillainMajixWidget;
		WBPFillainMajixWidget->UpdateMajixBar(Fillain->AttributeComponent->GetMaxMajix());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OverlayWidget is null in InitializeOverlay"));
	}
	const FWidgetControllerParams WidgetControllerParams (PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	if (WidgetController && OverlayWidget)
	{
		OverlayWidget->SetWidgetController(WidgetController);
		WidgetController->BroadcastInitialValues();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ WidgetController or CharacterOverlayWidgetFixed is null! WidgetController: %s | Overlay: %s"),
			WidgetController ? *WidgetController->GetName() : TEXT("nullptr"),
			OverlayWidget ? *OverlayWidget->GetName() : TEXT("nullptr"));
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