#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WidgetControllers/HAFWidgetController.h"
#include "FillainHUD.generated.h"

class USpellMenuWidgetController;
class UAnnouncement;
struct FFillainWidgetControllerParams;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class UFillainAttributeMenuWidget;
class UEnemyAttributeMenuWidget;
class UHAFUserWidget;
class UCharacterOverlayFixed;
class UAbilitySystemComponent;
class UAttributeSet;
class UUserWidget;
struct FWidgetControllerParams;

/** Basic package for player crosshair settings */
USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	FHUDPackage()
		: CrosshairsCenter(nullptr)
		, CrosshairsLeft(nullptr)
		, CrosshairsRight(nullptr)
		, CrosshairsTop(nullptr)
		, CrosshairsBottom(nullptr)
		, CrosshairSpread(0.f)
		, CrosshairsColor(FLinearColor::White)
	{}

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere)
	float CrosshairSpread;

	UPROPERTY(EditAnywhere)
	FLinearColor CrosshairsColor;
};

/**
 * Centralized HUD class for player overlays and enemy hover widgets.
 */
UCLASS()
class HEROESANDFILLAINS_API AFillainHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

	void AddCharacterOverlayFixed();
	void AddAnnouncement();
	void AddEliminationAnnouncement(FString Killer, FString Victim);
	void InitializeOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	// --- Widget controller getters ---
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& Params);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& Params);
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& Params);
	// --- Widgets ---
	UPROPERTY()
	TObjectPtr<UHAFUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UHAFUserWidget> OverlayWidgetClass;

	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UCharacterOverlayFixed> CharacterOverlayWidgetFixedClass;

	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UEnemyAttributeMenuWidget> EnemyAttributeMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UCharacterOverlayFixed> CharacterOverlayWidgetFixed;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWidgetControllerParams WidgetControllerParams;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	TObjectPtr<UAnnouncement> Announcement;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEliminationAnnouncement> EliminationAnnouncementClass;

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);
	void EliminationAnnouncementTimerFinished(UEliminationAnnouncement* MessageToRemove);

	UPROPERTY()
	APlayerController* OwningPlayer = nullptr;

	UPROPERTY()
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere)
	float EliminationAnnouncementTime = 2.5f;

	UPROPERTY()
	TArray<UEliminationAnnouncement*> EliminationMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

};
