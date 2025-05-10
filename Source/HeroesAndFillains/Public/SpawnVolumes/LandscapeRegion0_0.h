// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LandscapeRegion0_0.generated.h"

UCLASS()
class HEROESANDFILLAINS_API ALandscapeRegion0_0 : public AActor
{
	GENERATED_BODY()
	
public:
    ALandscapeRegion0_0();

    UPROPERTY(EditAnywhere, Category = "Spawning")
    bool bShouldSpawnWeapons = false;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UBoxComponent* SpawnBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> AssaultRifleToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> RocketLauncherToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> PistolToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> SMGToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> ShotgunToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> SniperRifleToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Weapons")
    TSubclassOf<AActor> GrenadeLauncherToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> ARAmmoToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> RocketsToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> PistolAmmoToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> SMGAmmoToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> ShotgunShellsToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> SniperAmmoToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Ammo")
    TSubclassOf<AActor> LaunchedGrenadesToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 SpawnCount = 100;

    UFUNCTION(BlueprintCallable)
    void SpawnAssaultRifles();

    UFUNCTION(BlueprintCallable)
    void SpawnRocketLaunchers();

    UFUNCTION(BlueprintCallable)
    void SpawnPistols();

    UFUNCTION(BlueprintCallable)
    void SpawnSMGs();

    UFUNCTION(BlueprintCallable)
    void SpawnShotguns();

    UFUNCTION(BlueprintCallable)
    void SpawnSniperRifles();

    UFUNCTION(BlueprintCallable)
    void SpawnGrenadeLaunchers();

    UFUNCTION(BlueprintCallable)
    void SpawnARAmmo();

    UFUNCTION(BlueprintCallable)
    void SpawnRockets();

    UFUNCTION(BlueprintCallable)
    void SpawnPistolAmmo();

    UFUNCTION(BlueprintCallable)
    void SpawnSMGAmmo();

    UFUNCTION(BlueprintCallable)
    void SpawnShotgunShells();

    UFUNCTION(BlueprintCallable)
    void SpawnSniperAmmo();

    UFUNCTION(BlueprintCallable)
    void SpawnLaunchedGrenades();


    FVector RandomBoxPoints() const;

private:
    UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class UPickupWidget> PickupWidgetClass;
	
    UFUNCTION()
    void AttachFloatingIcon(AActor* TargetWeapon);
	

};
