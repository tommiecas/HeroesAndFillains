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

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UBoxComponent* SpawnBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AActor> AssaultRifleToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AActor> RocketLauncherToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 SpawnCount = 50;

    UFUNCTION(BlueprintCallable)
    void SpawnAssaultRifles();

    UFUNCTION(BlueprintCallable)
    void SpawnRocketLaunchers();


    FVector RandomBoxPoints() const;

private:
    UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class UPickupWidget> PickupWidgetClass;
	
    UFUNCTION()
    void AttachFloatingIcon(AActor* TargetWeapon);
	

};
