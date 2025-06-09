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
#include "HUD/PickupGearWidget.h"
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
    // Delay the update to next frame to batch visibility changes
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, bShowWidgets]()
    {
        TArray<UWidgetComponent*> Widgets = {
            PickupGearWidgetComponent,
            ItemInfoWidgetComponent
        };

        for (auto* Widget : Widgets)
        {
            if (IsValid(Widget))
            {
                Widget->SetVisibility(bShowWidgets);
            }
        }
    });
}

void ALandscapeRegion0_0::BeginPlay()
{
    Super::BeginPlay();

    // Debug visualization - keep this but make it conditional
    if (SpawnBox)
    {
#if WITH_EDITOR
        FVector Origin = SpawnBox->Bounds.Origin;
        FVector Extent = SpawnBox->Bounds.BoxExtent;
        UE_LOG(LogTemp, Warning, TEXT("Spawn Box Bounds - Origin: %s, Extent: %s"),
            *Origin.ToString(), *Extent.ToString());

        DrawDebugBox(GetWorld(), Origin, Extent, FColor::Red, true, -1.f, 0, 5.f);
#endif
    }

    if (!bShouldSpawnWeapons) return;

#if WITH_EDITOR
    if (!GetWorld()->IsPlayInEditor()) return;
#endif

    // Stagger spawns across frames
    float SpawnDelay = 0.0f;
    const float DelayBetweenSpawns = 0.1f;

    //Spawn Ranged Weapons with delay
    for (const FSpawnVolumes& RangedEntry : WeaponSpawnList)
    {
        if (RangedEntry.ActorClass)
        {
            FTimerHandle SpawnTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
            SpawnTimerHandle,
            [this, RangedEntry]()
            {
                SpawnActorInBox(RangedEntry.ActorClass, {}, RangedEntry.DebugColor, RangedEntry.WidgetClass, nullptr);
            },
            SpawnDelay,
            false
        );
        SpawnDelay += DelayBetweenSpawns;
    }
        // Spawn ammo with additional delay
        for (const FSpawnVolumes& AmmoEntry : AmmoSpawnList)
        {
            if (AmmoEntry.ActorClass)
            {
                FTimerHandle SpawnTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    SpawnTimerHandle,
                    [this, AmmoEntry]()
                    {
                        SpawnActorInBox(AmmoEntry.ActorClass, {}, AmmoEntry.DebugColor, nullptr);
                    },
                    SpawnDelay,
                    false
                );
                SpawnDelay += DelayBetweenSpawns;
            }
        }

        // Spawn melee weapons with additional delay
        for (const FSpawnVolumes& MeleeEntry : MeleeSpawnList)
        {
            if (MeleeEntry.ActorClass)
            {
                FTimerHandle SpawnTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    SpawnTimerHandle,
                    [this, MeleeEntry]()
                    {
                        SpawnActorInBox(MeleeEntry.ActorClass, {}, MeleeEntry.DebugColor, nullptr);
                    },
                    SpawnDelay,
                    false
                );
                SpawnDelay += DelayBetweenSpawns;
            }
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

    // Initial delay to ensure actor is fully spawned
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, TargetActor, WidgetClass]()
    {
        if (!IsValid(TargetActor)) return;

        if (AWeaponBase* Weapon = Cast<AWeaponBase>(TargetActor))
        {
            // Setup widgets with delays between operations
            if (Weapon->PickupGearWidgetComponent)
            {
                Weapon->PickupGearWidgetComponent->SetVisibility(false);
                
                // Delay widget class assignment
                GetWorld()->GetTimerManager().SetTimerForNextTick([Weapon, WidgetClass]()
                {
                    if (!IsValid(Weapon)) return;
                    Weapon->PickupGearWidgetComponent->SetWidgetClass(WidgetClass);
                    
                    // Delay visibility change
                    FTimerHandle VisibilityTimerHandle;
                    Weapon->GetWorld()->GetTimerManager().SetTimer(
                        VisibilityTimerHandle,
                        [Weapon]()
                        {
                            if (IsValid(Weapon) && Weapon->PickupGearWidgetComponent)
                            {
                                Weapon->PickupGearWidgetComponent->SetVisibility(false);
                            }
                        },
                        0.1f,
                        false
                    );
                });
            }

            // Setup ItemInfo widget with separate delay
            if (Weapon->ItemInfoWidgetComponent)
            {
                Weapon->ItemInfoWidgetComponent->SetVisibility(false);
                
                GetWorld()->GetTimerManager().SetTimerForNextTick([Weapon]()
                {
                    if (!IsValid(Weapon)) return;
                    Weapon->ItemInfoWidgetComponent->SetWidgetClass(Weapon->ItemInfoWidgetClass);
                    
                    FTimerHandle VisibilityTimerHandle;
                    Weapon->GetWorld()->GetTimerManager().SetTimer(
                        VisibilityTimerHandle,
                        [Weapon]()
                        {
                            if (IsValid(Weapon) && Weapon->ItemInfoWidgetComponent)
                            {
                                Weapon->ItemInfoWidgetComponent->SetVisibility(false);
                            }
                        },
                        0.15f,  // Slightly delayed from PickupGear widget
                        false
                    );
                });
            }
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
    ShowPickupsAndInfoWidgets(false);
}
