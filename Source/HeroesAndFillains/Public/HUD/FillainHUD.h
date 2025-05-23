// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FillainHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AFillainHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
	void AddAnnouncement();
	void AddEliminationAnnouncement(FString Killer, FString Victim);


	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	class APlayerController* OwningPlayer;

	UPROPERTY()
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread/*, FLinearColor CrosshairsColor*/);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEliminationAnnouncement> EliminationAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float EliminationAnnouncementTime = 2.5f;

	UFUNCTION()
	void EliminationAnnouncementTimerFinished(UEliminationAnnouncement* MessageToRemove);

	UPROPERTY()
	TArray<UEliminationAnnouncement*> EliminationMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
