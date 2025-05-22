// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"      // For UBoxComponent
#include "Blueprint/UserWidget.h"         // For UUserWidget
#include "Templates/Function.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"

#include "LandscapeRegion0_0.generated.h"

class UWidgetComponent;

USTRUCT(BlueprintType)
struct FSpawnVolumes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnConfig")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = "SpawnConfig")
	FColor DebugColor = FColor::White;

	UPROPERTY(EditAnywhere, Category = "SpawnConfig")
	FString Label;

	UPROPERTY(EditAnywhere, Category = "SpawnConfig")
	TSubclassOf<UUserWidget> WidgetClass; 
};

UCLASS()
class HEROESANDFILLAINS_API ALandscapeRegion0_0 : public AActor
{
	GENERATED_BODY()
    
public:
	ALandscapeRegion0_0();

	void ShowPickupsAndInfoWidgets(bool bShowWidgets);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* PickupGearWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* PickupGearWidgetComponentB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentB;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FSpawnVolumes> WeaponSpawnList;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FSpawnVolumes> AmmoSpawnList;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<FSpawnVolumes> MeleeSpawnList;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 SpawnCount = 10;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	bool bShouldSpawnWeapons = true;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<UUserWidget> PickupWidgetClass;

	FVector RandomBoxPoints() const;
	bool IsValidSpawnPoint(const FVector& Location, FHitResult& GroundHit);
	void AttachFloatingIcon(class AActor* TargetActor, TSubclassOf<UUserWidget> WidgetClass);

	void SpawnActorInBox(TSubclassOf<AActor> ActorToSpawn, const TArray<FString>& FilterStrings, FColor DebugColor, TSubclassOf<UUserWidget> WidgetClass, TFunction<void(AActor*)> OnSpawnedSetup = nullptr);
};

