	#include "UI/FillainHUD.h"
    #include "UI/WidgetControllers/OverlayWidgetController.h"
    #include "UI/WidgetControllers/AttributeMenuWidgetController.h"
    #include "UI/Widgets/HAFUserWidget.h"
    #include "UI/Widgets/EnemyAttributeMenuWidget.h"
    #include "UI/CharacterOverlayFixed.h"
    #include "UI/Announcement.h"
    #include "UI/EliminationAnnouncement.h"
    #include "AbilitySystem/HAFAttributeSet.h"
    #include "PlayerState/HAFPlayerState.h"
    #include "Kismet/GameplayStatics.h"
    #include "Blueprint/WidgetLayoutLibrary.h"
    #include "Components/CanvasPanelSlot.h"
    #include "Components/HorizontalBox.h"
    #include "GameFramework/PlayerController.h"
    #include "GameFramework/PlayerState.h"
    #include "Engine/Canvas.h"
    #include "Engine/Texture2D.h"
    #include "UI/WidgetControllers/HAFWidgetController.h"
    #include "UI/WidgetControllers/HAFWidgetController.h"
    
    
    void AFillainHUD::BeginPlay()
    {
    	Super::BeginPlay();
    
    	APlayerController* PC = GetOwningPlayerController();
    	if (!PC)
    	{
    		UE_LOG(LogTemp, Error, TEXT("AFillainHUD::BeginPlay - No valid PlayerController found"));
    		return;
    	}
    
    	// Optionally pre-create an Enemy Hover widget pool later here if needed
    }
    
void AFillainHUD::InitializeOverlay(APlayerController* PC, APlayerState* PS, 
								UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class not set on BP_FillainHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class not set on BP_FillainHUD"));

	if (!OverlayWidgetController)
	{
		const FWidgetControllerParams Params(PC, PS, ASC, AS);
		OverlayWidgetController = GetOverlayWidgetController(Params);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksToDependencies();
		WidgetControllerParams = Params;
	}

	if (!OverlayWidget)
	{
		OverlayWidget = CreateWidget<UHAFUserWidget>(PC, OverlayWidgetClass);
		if (!OverlayWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("AFillainHUD: Failed to create OverlayWidget."));
			return;
		}
	}

	if (OverlayWidget && OverlayWidgetController)
	{
		// UE_LOG(LogTemp, Warning, TEXT("OverlayWidgetController valid? %s"), *GetNameSafe(OverlayWidgetController));

		OverlayWidget->SetWidgetController(OverlayWidgetController);

		if (!OverlayWidget->IsInViewport())
		{
			OverlayWidget->AddToViewport(200);
			OverlayWidget->SetVisibility(ESlateVisibility::Visible);
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

UAttributeMenuWidgetController* AFillainHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& Params)
{
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(Params);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

void AFillainHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		if (Announcement)
			Announcement->AddToViewport();
	}
}

void AFillainHUD::AddCharacterOverlayFixed()
{
	// Implement as needed later
}

void AFillainHUD::AddEliminationAnnouncement(FString Killer, FString Victim)
{
	OwningPlayer = OwningPlayer ? OwningPlayer : GetOwningPlayerController();
	if (!OwningPlayer || !EliminationAnnouncementClass) return;

	UEliminationAnnouncement* Widget = CreateWidget<UEliminationAnnouncement>(OwningPlayer, EliminationAnnouncementClass);
	if (!Widget) return;

	Widget->SetEliminationAnnouncementText(Killer, Victim);
	Widget->AddToViewport();

	// Slide older messages up
	for (UEliminationAnnouncement* Message : EliminationMessages)
	{
		if (Message && Message->AnnouncementBox)
		{
			if (UCanvasPanelSlot* Slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Message->AnnouncementBox))
			{
				FVector2D Pos = Slot->GetPosition();
				Slot->SetPosition({Pos.X, Pos.Y - Slot->GetSize().Y});
			}
		}
	}

	EliminationMessages.Add(Widget);
	FTimerHandle Timer;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("EliminationAnnouncementTimerFinished"), Widget);
	GetWorldTimerManager().SetTimer(Timer, Delegate, EliminationAnnouncementTime, false);
}

void AFillainHUD::EliminationAnnouncementTimerFinished(UEliminationAnnouncement* MessageToRemove)
{
	if (MessageToRemove)
		MessageToRemove->RemoveFromParent();
}

void AFillainHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D Center(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float Spread = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		auto Draw = [&](UTexture2D* Tex, FVector2D Offset)
		{
			if (Tex) DrawCrosshair(Tex, Center, Offset);
		};

		Draw(HUDPackage.CrosshairsCenter, {0, 0});
		Draw(HUDPackage.CrosshairsLeft, {-Spread, 0});
		Draw(HUDPackage.CrosshairsRight, {Spread, 0});
		Draw(HUDPackage.CrosshairsTop, {0, -Spread});
		Draw(HUDPackage.CrosshairsBottom, {0, Spread});
	}
}

void AFillainHUD::DrawCrosshair(UTexture2D* Texture, FVector2D Center, FVector2D Offset)
{
	if (!Texture) return;
	const float W = Texture->GetSizeX();
	const float H = Texture->GetSizeY();
	const FVector2D Pos(Center.X - W / 2.f + Offset.X, Center.Y - H / 2.f + Offset.Y);
	DrawTexture(Texture, Pos.X, Pos.Y, W, H, 0, 0, 1, 1, FLinearColor::White);
}
