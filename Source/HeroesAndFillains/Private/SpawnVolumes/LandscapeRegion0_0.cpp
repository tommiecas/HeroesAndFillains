// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolumes/LandscapeRegion0_0.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "WeaponsFinal/WeaponFinal.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/PickupWidget.h"
#include "HUD/PickupWidgetComponent.h"

ALandscapeRegion0_0::ALandscapeRegion0_0()
{
    PrimaryActorTick.bCanEverTick = false;

    SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
    RootComponent = SpawnBox;
}

void ALandscapeRegion0_0::BeginPlay()
{
    Super::BeginPlay();
    SpawnAssaultRifles();

}

FVector ALandscapeRegion0_0::RandomBoxPoints() const
{
    FVector Origin = SpawnBox->Bounds.Origin;
    FVector Extent = SpawnBox->Bounds.BoxExtent;
    return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

void ALandscapeRegion0_0::SpawnAssaultRifles()
{
    int32 Spawned = 0;

    while (Spawned < SpawnCount)
    {
        FVector RandomXY = RandomBoxPoints();
        FVector Start = FVector(RandomXY.X, RandomXY.Y, SpawnBox->Bounds.Origin.Z + SpawnBox->Bounds.BoxExtent.Z + 1000.f);
        FVector End = FVector(RandomXY.X, RandomXY.Y, Start.Z - 5000.f); // Look far below

        FHitResult GroundHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            GroundHit,
            Start,
            End,
            ECC_WorldStatic,
            Params
        );

        if (bHit)
        {
            // --- COMPONENT NAME FILTERING ---
            if (GroundHit.Component.IsValid())
            {
                UPrimitiveComponent* HitComp = GroundHit.Component.Get();
                FString CompName = HitComp->GetName();
                UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *CompName);

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush"))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Skipped due to component name: %s"), *CompName);
                    continue; // Skip this spawn point
                }

                // Optional: Check Material too
                UMaterialInterface* HitMat = HitComp->GetMaterial(0);
                if (HitMat)
                {
                    FString MatName = HitMat->GetName();
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush"))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Skipped due to material name: %s"), *MatName);
                        continue;
                    }
                }
            }

            // Now offset spawn above ground
            FVector SpawnLocation = GroundHit.ImpactPoint + FVector(0.f, 0.f, 100.f);

            // Optional overlap test
            FHitResult OverlapCheck;
            bool bBlocked = GetWorld()->SweepSingleByChannel(
                OverlapCheck,
                SpawnLocation,
                SpawnLocation,
                FQuat::Identity,
                ECC_WorldStatic,
                FCollisionShape::MakeSphere(25.f)
            );

            if (!bBlocked && AssaultRifleToSpawn)
            {
                GetWorld()->SpawnActor<AActor>(AssaultRifleToSpawn, SpawnLocation, FRotator::ZeroRotator);
                ++Spawned;

                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Green, true, 10.f);

            }
        }
    }
}

