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
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"

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
    SpawnRocketLaunchers();

}

FVector ALandscapeRegion0_0::RandomBoxPoints() const
{
    FVector Origin = SpawnBox->Bounds.Origin;
    FVector Extent = SpawnBox->Bounds.BoxExtent;
    return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

void ALandscapeRegion0_0::AttachFloatingIcon(AActor* TargetWeapon)
{
    if (!TargetWeapon) return;

    UPickupWidgetComponent* RandomizedWeaponWidgetComponent = NewObject<UPickupWidgetComponent>(TargetWeapon);
    if (RandomizedWeaponWidgetComponent)
    {
        RandomizedWeaponWidgetComponent->RegisterComponent(); // Very important
        RandomizedWeaponWidgetComponent->AttachToComponent(TargetWeapon->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        RandomizedWeaponWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
        RandomizedWeaponWidgetComponent->SetDrawSize(FVector2D(100, 50));
        RandomizedWeaponWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // Float above rifle

        // Cast PickupWidgetComponentClass to TSubclassOf<UUserWidget> before assigning
        TSubclassOf<UUserWidget> WidgetClass = *PickupWidgetClass;
        RandomizedWeaponWidgetComponent->SetWidgetClass(PickupWidgetClass); // Assign your UUserWidget subclass here
    }
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
                
                FString CompName = GroundHit.Component->GetName();
                UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *CompName);

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("RocketLauncher"))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Skipped due to component name: %s"), *CompName);
                    continue; // Skip this spawn point
                }

                // Optional: Check Material too
                UPrimitiveComponent* HitComp = GroundHit.Component.Get();
                UMaterialInterface* HitMat = HitComp->GetMaterial(0);
                if (HitMat)
                {
                    FString MatName = HitMat->GetName();
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("RocketLauncher"))
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
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedAssaultRifle = GetWorld()->SpawnActor<AActor>(AssaultRifleToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedAssaultRifle)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* AssaultRifleFinal = Cast<AWeaponFinal>(SpawnedAssaultRifle); // Use your correct weapon base class

                    if (AssaultRifleFinal && AssaultRifleFinal->AreaSphere)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("CollisionSphere found and setup"));

                        AssaultRifleFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        AssaultRifleFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        AssaultRifleFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        AssaultRifleFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("CollisionSphere is MISSING!"));
                    }

                    if (AssaultRifleFinal->HoverLight)
                    {
						UE_LOG(LogTemp, Warning, TEXT("Rifle has HoverLight: %s"), *AssaultRifleFinal->HoverLight->GetName());
                    }
                    else
                    {
						UE_LOG(LogTemp, Error, TEXT("Rifle is MISSING HoverLight!"));
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(AssaultRifleFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    AssaultRifleFinal->bShouldHover = true;
                    AssaultRifleFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (AssaultRifleFinal->HoverLight)
                    {
                        AssaultRifleFinal->HoverLight->SetVisibility(true);
                    }

                    if (AssaultRifleFinal->HoverDecal)
                    {
                        AssaultRifleFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedAssaultRifle);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Green, true, 10.f);
                
            }
        }
    }
}

void ALandscapeRegion0_0::SpawnRocketLaunchers()
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

                FString CompName = GroundHit.Component->GetName();
                UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *CompName);

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle"))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Skipped due to component name: %s"), *CompName);
                    continue; // Skip this spawn point
                }

                // Optional: Check Material too
                UPrimitiveComponent* HitComp = GroundHit.Component.Get();
                UMaterialInterface* HitMat = HitComp->GetMaterial(0);
                if (HitMat)
                {
                    FString MatName = HitMat->GetName();
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle"))
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

            if (!bBlocked && RocketLauncherToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedRocketLauncher = GetWorld()->SpawnActor<AActor>(RocketLauncherToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedRocketLauncher)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* RocketLauncherFinal = Cast<AWeaponFinal>(SpawnedRocketLauncher); // Use your correct weapon base class

                    if (RocketLauncherFinal && RocketLauncherFinal->AreaSphere)
                    {
                        RocketLauncherFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        RocketLauncherFinal->AreaSphere->SetGenerateOverlapEvents(true);

                        // (Optional) Reset collision profile
                        RocketLauncherFinal->AreaSphere->SetCollisionProfileName(FName("OverlapAllDynamic"));
                    }

                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(RocketLauncherFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // (Optional) Auto-attach a floating widget above rifle
                    AttachFloatingIcon(SpawnedRocketLauncher);

                    ++Spawned;
                    DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Green, true, 10.f);
                }
            }
        }
    }
}

