// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LandscapeRegion0_0.generated.h"

class UBoxComponent;
class UUserWidget;
class UPickupWidgetComponent;

UCLASS()
class HEROESANDFILLAINS_API ALandscapeRegion0_0 : public AActor
{
	GENERATED_BODY()

public:
	ALandscapeRegion0_0();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnBox;

	UFUNCTION()
	FVector RandomBoxPoints() const;

	void AttachFloatingIcon(AActor* TargetWeapon);

	void SpawnActorInBox(TSubclassOf<AActor> ActorToSpawn, const TArray<FString>& FilterStrings, FColor DebugColor, TFunction<void(AActor*)> OnSpawnedSetup);

	// Spawners
	void SpawnAssaultRifles();
	void SpawnRocketLaunchers();
	void SpawnPistols();
	void SpawnShotguns();
	void SpawnSnipers();
	void SpawnSMGs();
	void SpawnGrenadeLaunchers();
	
	// Actor classes
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AssaultRifleToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> RocketLaunchersToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PistolToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ShotgunToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SnipersToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SMGToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> GrenadeLaunchersToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ARAmmoToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> RocketsToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PistolAmmoToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SniperAmmoToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ShotgunShellsToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LaunchedGrenadesToSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SMGAmmoToSpawn;
	
	void SpawnARAmmo();
	void SpawnRockets();
	void SpawnPistolAmmo();
	void SpawnShotgunShells();
	void SpawnSMGAmmo();
	void SpawnSniperAmmo();
	void SpawnLaunchedGrenades();
	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PickupWidgetClass;

	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 100;

	UPROPERTY(EditAnywhere)
	bool bShouldSpawnWeapons = true;
};
