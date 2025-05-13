// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolumes/LandscapeRegion0_0.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "HUD/PickupWidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickups/PickupSpawnPoint.h"
#include "WeaponsFinal/HitScanWeaponFinal.h"
#include "WeaponsFinal/WeaponFinal.h"

ALandscapeRegion0_0::ALandscapeRegion0_0()
{
    PrimaryActorTick.bCanEverTick = false;
    SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
    RootComponent = SpawnBox;
}

void ALandscapeRegion0_0::BeginPlay()
{
    Super::BeginPlay();

    if (!bShouldSpawnWeapons) return;

#if WITH_EDITOR
    if (!GetWorld()->IsPlayInEditor()) return;
#endif

    SpawnAssaultRifles();
    SpawnRocketLaunchers();
    SpawnPistols();
}

FVector ALandscapeRegion0_0::RandomBoxPoints() const
{
    return UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent);
}

void ALandscapeRegion0_0::AttachFloatingIcon(AActor* TargetWeapon)
{
    if (!TargetWeapon) return;
    
    UPickupWidgetComponent* Widget = NewObject<UPickupWidgetComponent>(TargetWeapon);
    if (Widget)
    {
        Widget->RegisterComponent();
        Widget->AttachToComponent(TargetWeapon->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        Widget->SetWidgetSpace(EWidgetSpace::World);
        Widget->SetDrawSize(FVector2D(100, 50));
        Widget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
        Widget->SetWidgetClass(PickupWidgetClass);

        if (AWeaponFinal* Weapon = Cast<AWeaponFinal>(TargetWeapon))
        {
            Weapon->FloatingWidgetComponent = Widget;
        }
    }
}

void ALandscapeRegion0_0::SpawnActorInBox(
    TSubclassOf<AActor> ActorToSpawn,
    const TArray<FString>& FilterStrings,
    FColor DebugColor,
    TFunction<void(AActor*)> OnSpawnedSetup)
{
    int32 Spawned = 0;
    while (Spawned < SpawnCount)
    {
        FVector RandomXY = RandomBoxPoints();
        FVector Start = RandomXY + FVector(0, 0, SpawnBox->Bounds.BoxExtent.Z + 1000.f);
        FVector End = Start - FVector(0, 0, 5000.f);

        FHitResult GroundHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_WorldStatic, Params))
        {
            if (!GroundHit.Component.IsValid()) continue;

            FString CompName = GroundHit.Component->GetName();
            if (FilterStrings.ContainsByPredicate([&](const FString& S){ return CompName.Contains(S); })) continue;

            if (UMaterialInterface* Mat = GroundHit.Component->GetMaterial(0))
            {
                if (FilterStrings.ContainsByPredicate([&](const FString& S){ return Mat->GetName().Contains(S); })) continue;
            }

            FVector SpawnLocation = GroundHit.ImpactPoint + FVector(0, 0, 100);
            FHitResult OverlapCheck;

            bool bBlocked = GetWorld()->SweepSingleByChannel(
                OverlapCheck, SpawnLocation, SpawnLocation,
                FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(25.f));

            if (!bBlocked && ActorToSpawn)
            {
                FActorSpawnParameters Prams;
                Prams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnLocation, FRotator::ZeroRotator, Prams);
                if (SpawnedActor)
                {
                    OnSpawnedSetup(SpawnedActor);
                    AttachFloatingIcon(SpawnedActor);
                    DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, DebugColor, true, 10.f);
                    ++SpawnedActor;
                }
            }
        }
    }
}

void ALandscapeRegion0_0::SpawnAssaultRifles()
{
    SpawnActorInBox(AssaultRifleToSpawn, {"Tree", "Rock", "Bush", "Boulder"}, FColor::Blue,
        [](AActor* Actor) {
            if (AWeaponFinal* Weapon = Cast<AWeaponFinal>(Actor))
            {
                if (Weapon->AreaSphere)
                {
                    Weapon->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                    Weapon->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                    Weapon->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                    Weapon->AreaSphere->SetGenerateOverlapEvents(true);
                }

                if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Weapon->GetRootComponent()))
                {
                    Root->SetSimulatePhysics(false);
                    Root->SetEnableGravity(false);
                }

                Weapon->bShouldHover = true;
                Weapon->bShouldFloatSpin = true;

                if (Weapon->HoverLight) Weapon->HoverLight->SetVisibility(true);
                if (Weapon->HoverDecal) Weapon->HoverDecal->SetVisibility(true);
            }
        });
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

            if (!bBlocked && RocketLaunchersToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedRocketLauncher = GetWorld()->SpawnActor<AActor>(RocketLaunchersToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedRocketLauncher)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* RocketLauncherFinal = Cast<AWeaponFinal>(SpawnedRocketLauncher); // Use your correct weapon base class

                    if (RocketLauncherFinal && RocketLauncherFinal->AreaSphere)
                    {

                        RocketLauncherFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        RocketLauncherFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        RocketLauncherFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        RocketLauncherFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (RocketLauncherFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(RocketLauncherFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    RocketLauncherFinal->bShouldHover = true;
                    RocketLauncherFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (RocketLauncherFinal->HoverLight)
                    {
                        RocketLauncherFinal->HoverLight->SetVisibility(true);
                    }

                    if (RocketLauncherFinal->HoverDecal)
                    {
                        RocketLauncherFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedRocketLauncher);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Red, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnPistols()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher"))
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

            if (!bBlocked && PistolToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedPistol = GetWorld()->SpawnActor<AActor>(PistolToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedPistol)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AHitScanWeaponFinal* PistolFinal = Cast<AHitScanWeaponFinal>(SpawnedPistol); // Use your correct weapon base class

                    if (PistolFinal && PistolFinal->AreaSphere)
                    {

                        PistolFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        PistolFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        PistolFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        PistolFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (PistolFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(PistolFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    PistolFinal->bShouldHover = true;
                    PistolFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (PistolFinal->HoverLight)
                    {
                        PistolFinal->HoverLight->SetVisibility(true);
                    }

                    if (PistolFinal->HoverDecal)
                    {
                        PistolFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedPistol);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Yellow, true, 10.f);

            }
        }

    }
}
void ALandscapeRegion0_0::SpawnSMGs()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("RocketLauncher") || CompName.Contains("AssaultRifle") || CompName.Contains("Pistol"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("RocketLauncher") || CompName.Contains("AssaultRifle") || CompName.Contains("Pistol"))
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

            if (!bBlocked && SMGToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedSMG = GetWorld()->SpawnActor<AActor>(SMGToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedSMG)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* SMGFinal = Cast<AWeaponFinal>(SpawnedSMG); // Use your correct weapon base class

                    if (SMGFinal && SMGFinal->AreaSphere)
                    {

                        SMGFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        SMGFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        SMGFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        SMGFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (SMGFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SMGFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    SMGFinal->bShouldHover = true;
                    SMGFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (SMGFinal->HoverLight)
                    {
                        SMGFinal->HoverLight->SetVisibility(true);
                    }

                    if (SMGFinal->HoverDecal)
                    {
                        SMGFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedSMG);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Green, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnShotguns()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG"))
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

            if (!bBlocked && ShotgunToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedShotgun = GetWorld()->SpawnActor<AActor>(ShotgunToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedShotgun)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* ShotgunFinal = Cast<AWeaponFinal>(SpawnedShotgun); // Use your correct weapon base class

                    if (ShotgunFinal && ShotgunFinal->AreaSphere)
                    {

                        ShotgunFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        ShotgunFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        ShotgunFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        ShotgunFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (ShotgunFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(ShotgunFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    ShotgunFinal->bShouldHover = true;
                    ShotgunFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (ShotgunFinal->HoverLight)
                    {
                        ShotgunFinal->HoverLight->SetVisibility(true);
                    }

                    if (ShotgunFinal->HoverDecal)
                    {
                        ShotgunFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedShotgun);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Purple, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnSnipers()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("GrenadeLauncher"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("GrenadeLauncher"))
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

            if (!bBlocked && SnipersToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedSniperRifle = GetWorld()->SpawnActor<AActor>(SnipersToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedSniperRifle)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* SniperRifleFinal = Cast<AWeaponFinal>(SpawnedSniperRifle); // Use your correct weapon base class

                    if (SniperRifleFinal && SniperRifleFinal->AreaSphere)
                    {
                        SniperRifleFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        SniperRifleFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        SniperRifleFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        SniperRifleFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (SniperRifleFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SniperRifleFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    SniperRifleFinal->bShouldHover = true;
                    SniperRifleFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (SniperRifleFinal->HoverLight)
                    {
                        SniperRifleFinal->HoverLight->SetVisibility(true);
                    }

                    if (SniperRifleFinal->HoverDecal)
                    {
                        SniperRifleFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedSniperRifle);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::White, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnGrenadeLaunchers()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && GrenadeLaunchersToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedGrenadeLauncher = GetWorld()->SpawnActor<AActor>(GrenadeLaunchersToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedGrenadeLauncher)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    AWeaponFinal* GrenadeLauncherFinal = Cast<AWeaponFinal>(SpawnedGrenadeLauncher); // Use your correct weapon base class

                    if (GrenadeLauncherFinal && GrenadeLauncherFinal->AreaSphere)
                    {

                        GrenadeLauncherFinal->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        GrenadeLauncherFinal->AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        GrenadeLauncherFinal->AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        GrenadeLauncherFinal->AreaSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (GrenadeLauncherFinal->HoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(GrenadeLauncherFinal->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    GrenadeLauncherFinal->bShouldHover = true;
                    GrenadeLauncherFinal->bShouldFloatSpin = true;

                    // Optional: check light/visuals
                    if (GrenadeLauncherFinal->HoverLight)
                    {
                        GrenadeLauncherFinal->HoverLight->SetVisibility(true);
                    }

                    if (GrenadeLauncherFinal->HoverDecal)
                    {
                        GrenadeLauncherFinal->HoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedGrenadeLauncher);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Orange, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnARAmmo()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && ARAmmoToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedARAmmo = GetWorld()->SpawnActor<AActor>(ARAmmoToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedARAmmo)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* ARAmmoSpawnPoint = Cast<APickupSpawnPoint>(SpawnedARAmmo); // Use your correct weapon base class

                    if (ARAmmoSpawnPoint && ARAmmoSpawnPoint->PickupSphere)
                    {

                        ARAmmoSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        ARAmmoSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        ARAmmoSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        ARAmmoSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (ARAmmoSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(ARAmmoSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    ARAmmoSpawnPoint->bShouldPickupHover = true;
                    ARAmmoSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (ARAmmoSpawnPoint->PickupHoverLight)
                    {
                        ARAmmoSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (ARAmmoSpawnPoint->PickupHoverDecal)
                    {
                        ARAmmoSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedARAmmo);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Blue, true, 10.f);

            }
        }
    }
}
void ALandscapeRegion0_0::SpawnRockets()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && RocketsToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AActor* SpawnedRockets = GetWorld()->SpawnActor<AActor>(RocketsToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedRockets)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* RocketSpawnPoint = Cast<APickupSpawnPoint>(SpawnedRockets); // Use your correct weapon base class

                    if (RocketSpawnPoint && RocketSpawnPoint->PickupSphere)
                    {

                        RocketSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        RocketSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        RocketSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        RocketSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (RocketSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(RocketSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    RocketSpawnPoint->bShouldPickupHover = true;
                    RocketSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (RocketSpawnPoint->PickupHoverLight)
                    {
                        RocketSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (RocketSpawnPoint->PickupHoverDecal)
                    {
                        RocketSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedRockets);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Red, true, 10.f);

            }
        }
    }
}

void ALandscapeRegion0_0::SpawnPistolAmmo()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && PistolAmmoToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                AActor* SpawnedPistolAmmo = GetWorld()->SpawnActor<AActor>(PistolAmmoToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedPistolAmmo)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* PistolAmmoSpawnPoint = Cast<APickupSpawnPoint>(SpawnedPistolAmmo); // Use your correct weapon base class

                    if (PistolAmmoSpawnPoint && PistolAmmoSpawnPoint->PickupSphere)
                    {

                        PistolAmmoSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        PistolAmmoSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        PistolAmmoSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        PistolAmmoSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (PistolAmmoSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(PistolAmmoSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    PistolAmmoSpawnPoint->bShouldPickupHover = true;
                    PistolAmmoSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (PistolAmmoSpawnPoint->PickupHoverLight)
                    {
                        PistolAmmoSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (PistolAmmoSpawnPoint->PickupHoverDecal)
                    {
                        PistolAmmoSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedPistolAmmo);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Yellow, true, 10.f);

            }
        }
    }
}

void ALandscapeRegion0_0::SpawnSMGAmmo()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && SMGAmmoToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                AActor* SpawnedSMGAmmo = GetWorld()->SpawnActor<AActor>(SMGAmmoToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedSMGAmmo)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* SMGAmmoSpawnPoint = Cast<APickupSpawnPoint>(SpawnedSMGAmmo); // Use your correct weapon base class

                    if (SMGAmmoSpawnPoint && SMGAmmoSpawnPoint->PickupSphere)
                    {

                        SMGAmmoSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        SMGAmmoSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        SMGAmmoSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        SMGAmmoSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (SMGAmmoSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SMGAmmoSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    SMGAmmoSpawnPoint->bShouldPickupHover = true;
                    SMGAmmoSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (SMGAmmoSpawnPoint->PickupHoverLight)
                    {
                        SMGAmmoSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (SMGAmmoSpawnPoint->PickupHoverDecal)
                    {
                        SMGAmmoSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedSMGAmmo);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Green, true, 10.f);

            }
        }
    }
}

void ALandscapeRegion0_0::SpawnShotgunShells()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && ShotgunShellsToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                AActor* SpawnedShotgunShells = GetWorld()->SpawnActor<AActor>(ShotgunShellsToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedShotgunShells)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* ShotgunShellsSpawnPoint = Cast<APickupSpawnPoint>(SpawnedShotgunShells); // Use your correct weapon base class

                    if (ShotgunShellsSpawnPoint && ShotgunShellsSpawnPoint->PickupSphere)
                    {

                        ShotgunShellsSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        ShotgunShellsSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        ShotgunShellsSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        ShotgunShellsSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (ShotgunShellsSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(ShotgunShellsSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    ShotgunShellsSpawnPoint->bShouldPickupHover = true;
                    ShotgunShellsSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (ShotgunShellsSpawnPoint->PickupHoverLight)
                    {
                        ShotgunShellsSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (ShotgunShellsSpawnPoint->PickupHoverDecal)
                    {
                        ShotgunShellsSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedShotgunShells);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Purple, true, 10.f);

            }
        }
    }

}

void ALandscapeRegion0_0::SpawnSniperAmmo()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && SniperAmmoToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                AActor* SpawnedSniperAmmo = GetWorld()->SpawnActor<AActor>(SniperAmmoToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedSniperAmmo)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* SniperAmmoSpawnPoint = Cast<APickupSpawnPoint>(SpawnedSniperAmmo); // Use your correct weapon base class

                    if (SniperAmmoSpawnPoint && SniperAmmoSpawnPoint->PickupSphere)
                    {

                        SniperAmmoSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        SniperAmmoSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        SniperAmmoSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        SniperAmmoSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (SniperAmmoSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SniperAmmoSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    SniperAmmoSpawnPoint->bShouldPickupHover = true;
                    SniperAmmoSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (SniperAmmoSpawnPoint->PickupHoverLight)
                    {
                        SniperAmmoSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (SniperAmmoSpawnPoint->PickupHoverDecal)
                    {
                        SniperAmmoSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedSniperAmmo);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::White, true, 10.f);

            }
        }
    }

}

void ALandscapeRegion0_0::SpawnLaunchedGrenades()
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

                if (CompName.Contains("Tree") || CompName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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
                    if (MatName.Contains("Tree") || MatName.Contains("Rock") || CompName.Contains("Boulder") || CompName.Contains("Bush") || CompName.Contains("AssaultRifle") || CompName.Contains("RocketLauncher") || CompName.Contains("Pistol") || CompName.Contains("SubmachineGun") || CompName.Contains("SMG") || CompName.Contains("Shotgun") || CompName.Contains("SniperRifle"))
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

            if (!bBlocked && LaunchedGrenadesToSpawn)
            {
                // --- Use proper spawn parameters ---
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                AActor* SpawnedLaunchedGrenades = GetWorld()->SpawnActor<AActor>(LaunchedGrenadesToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

                if (SpawnedLaunchedGrenades)
                {
                    // --- After spawn, force collision settings ---
                    // Cast to your weapon class if needed
                    APickupSpawnPoint* LaunchedGrenadesSpawnPoint = Cast<APickupSpawnPoint>(SpawnedLaunchedGrenades); // Use your correct weapon base class

                    if (LaunchedGrenadesSpawnPoint && LaunchedGrenadesSpawnPoint->PickupSphere)
                    {

                        LaunchedGrenadesSpawnPoint->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                        LaunchedGrenadesSpawnPoint->PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
                        LaunchedGrenadesSpawnPoint->PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
                        LaunchedGrenadesSpawnPoint->PickupSphere->SetGenerateOverlapEvents(true);
                    }
                    else
                    {
                    }

                    if (LaunchedGrenadesSpawnPoint->PickupHoverLight)
                    {
                    }
                    else
                    {
                    }
                    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(LaunchedGrenadesSpawnPoint->GetRootComponent());
                    if (RootPrimitive)
                    {
                        RootPrimitive->SetSimulatePhysics(false);
                        RootPrimitive->SetEnableGravity(false);
                    }
                    // Ensure float behavior is active
                    LaunchedGrenadesSpawnPoint->bShouldPickupHover = true;
                    LaunchedGrenadesSpawnPoint->bShouldPickupFloatSpin = true;

                    // Optional: check light/visuals
                    if (LaunchedGrenadesSpawnPoint->PickupHoverLight)
                    {
                        LaunchedGrenadesSpawnPoint->PickupHoverLight->SetVisibility(true);
                    }

                    if (LaunchedGrenadesSpawnPoint->PickupHoverDecal)
                    {
                        LaunchedGrenadesSpawnPoint->PickupHoverDecal->SetVisibility(true);
                    }
                }
                // (Optional) Auto-attach a floating widget above rifle
                AttachFloatingIcon(SpawnedLaunchedGrenades);

                ++Spawned;
                DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 12, FColor::Orange, true, 10.f);

            }
        }
    }
};


