// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolumes/LandscapeRegion0_0.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "HUD/PickupWidgetComponent.h"
#include "Weapons/WeaponBase.h"
#include "CollisionQueryParams.h"
#include "LandscapeComponent.h"
#include "Engine/OverlapResult.h"             // 🔥 For FOverlapResult
#include "HUD/ItemInfoWidgetBase.h"
#include "Pickups/AmmoPickup.h"
#include "Pickups/PickupSpawnPoint.h"
#include "Templates/Function.h"               // 🔥 For TFunction
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"


ALandscapeRegion0_0::ALandscapeRegion0_0()
{
    PrimaryActorTick.bCanEverTick = false;
    SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
    RootComponent = SpawnBox;
}

void ALandscapeRegion0_0::ShowPickupsAndInfoWidgets(bool bShowWidgets)
{
    if (PickupGearWidgetComponentA) PickupGearWidgetComponentA->SetVisibility(bShowWidgets);
    if (PickupGearWidgetComponentB) PickupGearWidgetComponentB->SetVisibility(bShowWidgets);
    if (ItemInfoWidgetComponentA) ItemInfoWidgetComponentA->SetVisibility(bShowWidgets);
    if (ItemInfoWidgetComponentB) ItemInfoWidgetComponentB->SetVisibility(bShowWidgets);
}

void ALandscapeRegion0_0::BeginPlay()
{
    Super::BeginPlay();

    if (SpawnBox)
    {
        FVector Origin = SpawnBox->Bounds.Origin;
        FVector Extent = SpawnBox->Bounds.BoxExtent;
        UE_LOG(LogTemp, Warning, TEXT("Spawn Box Bounds - Origin: %s, Extent: %s"),
            *Origin.ToString(), *Extent.ToString());

        DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true, -1.f, 0, 5.f);
    }

    if (!bShouldSpawnWeapons) return;

#if WITH_EDITOR
    if (!GetWorld()->IsPlayInEditor()) return;
#endif

    for (const FSpawnVolumes& Entry : WeaponSpawnList)
    {
        if (Entry.ActorClass)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawning Weapon: %s"), *Entry.Label);
            SpawnActorInBox(Entry.ActorClass, {}, Entry.DebugColor, Entry.WidgetClass, nullptr);
        }
    }

    for (const FSpawnVolumes& Entry : AmmoSpawnList)
    {
        if (Entry.ActorClass)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawning Ammo: %s"), *Entry.Label);
            SpawnActorInBox(Entry.ActorClass, {}, Entry.DebugColor, nullptr);
        }
    }

    for (const FSpawnVolumes& Entry : MeleeSpawnList)
    {
        if (Entry.ActorClass)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawning Melee: %s"), *Entry.Label);
            SpawnActorInBox(Entry.ActorClass, {}, Entry.DebugColor, nullptr);
        }
    }
}

FVector ALandscapeRegion0_0::RandomBoxPoints() const
{
    return UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent);
}

void ALandscapeRegion0_0::AttachFloatingIcon(AActor* TargetActor, TSubclassOf<UUserWidget> WidgetClass)
{
    if (!TargetActor || !TargetActor->GetRootComponent() || !WidgetClass) return;

    GetWorld()->GetTimerManager().SetTimerForNextTick([this, TargetActor, WidgetClass]()
    {
        if (!IsValid(TargetActor)) return;

        UPickupWidgetComponent* Widget = NewObject<UPickupWidgetComponent>(TargetActor);
        if (Widget)
        {
            Widget->RegisterComponent();
            Widget->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            Widget->SetWidgetSpace(EWidgetSpace::World);
            Widget->SetDrawSize(FVector2D(100.f, 50.f));
            Widget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
            Widget->SetWidgetClass(WidgetClass);
        }
    });
}



bool ALandscapeRegion0_0::IsValidSpawnPoint(const FVector& Location, FHitResult& GroundHit)
{
    FVector Start = Location + FVector(0.f, 0.f, 1000.f);
    FVector End = Location - FVector(0.f, 0.f, 1000.f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Params);
    if (!bHit) return false;

    if (GroundHit.Component.IsValid())
    {
        FString CompName = GroundHit.Component->GetName();
        if (GroundHit.GetComponent()->IsA<ULandscapeComponent>())
        {
            TArray<FOverlapResult> Overlaps;
            FVector TestLocation = GroundHit.ImpactPoint + FVector(0.f, 0.f, 20.f);
            FCollisionShape SphereShape = FCollisionShape::MakeSphere(20.f);

            bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
                Overlaps, TestLocation, FQuat::Identity, ECC_WorldStatic, SphereShape, Params);

            return !bHasOverlaps;
        }
    }
    return false;
}

void ALandscapeRegion0_0::SpawnActorInBox(
    TSubclassOf<AActor> ActorToSpawn,
    const TArray<FString>& FilterStrings,
    FColor DebugColor,
    TSubclassOf<UUserWidget> WidgetClass,  
    TFunction<void(AActor*)> OnSpawnedSetup
)
{
    if (!ActorToSpawn || !SpawnBox)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid ActorToSpawn or SpawnBox is null"));
        return;
    }

    int32 SpawnedCount = 0;
    int32 MaxAttempts = 1000;
    int32 Attempts = 0;

    while (SpawnedCount < SpawnCount && Attempts < MaxAttempts)
    {
        ++Attempts;
        FVector SpawnLocation = RandomBoxPoints();
        FHitResult GroundHit;

        FVector TraceStart = SpawnLocation + FVector(0.f, 0.f, 5000.f);
        FVector TraceEnd = SpawnLocation - FVector(0.f, 0.f, 5000.f);

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

        if (bHit && GroundHit.Component.IsValid() && GroundHit.Component->GetName().Contains(TEXT("Landscape")))
        {
            FVector FinalLocation = GroundHit.ImpactPoint + FVector(0.f, 0.f, 20.f);
            TArray<FOverlapResult> Overlaps;
            FCollisionShape SphereShape = FCollisionShape::MakeSphere(10.f);

            bool bHasBlockingOverlap = false;
            for (const FOverlapResult& Result : Overlaps)
            {
                if (Result.GetActor() && Result.GetActor() != this)
                {
                    bHasBlockingOverlap = true;
                    break;
                }
            }
            if (!bHasBlockingOverlap)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                if (AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, FinalLocation, FRotator::ZeroRotator, SpawnParams))
                {
                    ++SpawnedCount;

                    if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(SpawnedActor))
                    {
                        MeleeWeapon->ItemInfoWidgetClass = WidgetClass; // 🔥 assign the class
                    }
                    else if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(SpawnedActor))
                    {
                        RangedWeapon->ItemInfoWidgetClass = WidgetClass;
                    }
                    else if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(SpawnedActor))
                    {
                        AmmoPickup->ItemInfoWidgetClass = WidgetClass;
                    }
                    
                    if (OnSpawnedSetup)
                    {
                        OnSpawnedSetup(SpawnedActor);
                    }
                    if (AWeaponBase* SpawnedWeapon = Cast<AWeaponBase>(SpawnedActor))
                    {
                        AttachFloatingIcon(SpawnedWeapon, WidgetClass);
                    }
                    DrawDebugSphere(GetWorld(), FinalLocation, 25.f, 12, DebugColor, false, 30.f);
                    UE_LOG(LogTemp, Log, TEXT("Spawned: %s at %s"), *ActorToSpawn->GetName(), *FinalLocation.ToString());
                }
            }
        }
    }
}
