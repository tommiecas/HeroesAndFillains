#include "HAFComponents/HiddenTreasureScannerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "HAFComponents/HiddenTreasureComponent.h"
#include "DrawDebugHelpers.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

UHiddenTreasureScannerComponent::UHiddenTreasureScannerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Create the sphere correctly at construction time
    ScanSphere = CreateDefaultSubobject<USphereComponent>(TEXT("IntuitionScanSphere"));

    // Base collision setup (correct to do here)
    ScanSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ScanSphere->SetCollisionObjectType(ECC_WorldDynamic);
    ScanSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    ScanSphere->SetGenerateOverlapEvents(true);
}

void UHiddenTreasureScannerComponent::OnRegister()
{
    Super::OnRegister();

    // Attach sphere to the owning actor’s root
    if (AActor* Owner = GetOwner())
    {
        if (USceneComponent* Root = Owner->GetRootComponent())
        {
            ScanSphere->AttachToComponent(
                Root,
                FAttachmentTransformRules::KeepRelativeTransform);
        }
    }

    // Final collision channel setup
    ScanSphere->SetCollisionResponseToChannel(ECC_Treasure, ECR_Overlap);

    // SAFE delegate binding
    if (!ScanSphere->OnComponentBeginOverlap.IsAlreadyBound(
        this, &UHiddenTreasureScannerComponent::OnScanBegin))
    {
        ScanSphere->OnComponentBeginOverlap.AddDynamic(
            this, &UHiddenTreasureScannerComponent::OnScanBegin);
    }

    if (!ScanSphere->OnComponentEndOverlap.IsAlreadyBound(
        this, &UHiddenTreasureScannerComponent::OnScanEnd))
    {
        ScanSphere->OnComponentEndOverlap.AddDynamic(
            this, &UHiddenTreasureScannerComponent::OnScanEnd);
    }
}

void UHiddenTreasureScannerComponent::OnScanBegin(
    UPrimitiveComponent* PrimitiveComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // Only local player should highlight treasure
    if (!IsLocal())
        return;

    // Apply intuition highlight / emissive scaling to treasure components on the actor
    ApplyToActorTreasures(OtherActor, CurrentIntuitionFraction);
}

void UHiddenTreasureScannerComponent::OnScanEnd(
    UPrimitiveComponent* PrimitiveComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    // Only local player should remove highlight
    if (!IsLocal())
        return;

    // Remove highlight/emissive scaling from treasure components
    ApplyToActorTreasures(OtherActor, 0.f);
}

void UHiddenTreasureScannerComponent::ApplyToActorTreasures(AActor* Other, float Fraction)
{
    if (!Other || Other == GetOwner())
        return;

    // Find any UHiddenTreasureComponent on the actor and update its intuition fraction
    for (auto* HT : TInlineComponentArray<UHiddenTreasureComponent*>(Other, true))
    {
        HT->ApplyIntuitionScale(Fraction);
    }
}

void UHiddenTreasureScannerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (const APawn* P = Cast<APawn>(GetOwner()))
    {
        if (!P->IsLocallyControlled())
        {
            if (ScanSphere)
            {
                ScanSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            return;
        }
    }

    UpdateRadius();

    TArray<AActor*> Overlapping;
    ScanSphere->GetOverlappingActors(Overlapping);

    for (AActor* A : Overlapping)
    {
        ApplyToActorTreasures(A, CurrentIntuitionFraction);
    }
}

void UHiddenTreasureScannerComponent::SetIntuitionFraction(float Fraction)
{
    CurrentIntuitionFraction = FMath::Clamp(Fraction, 0.f, 1.f);

    if (!ScanSphere || !ScanSphere->IsRegistered() ||
        ScanSphere->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
    {
        return;
    }

    UpdateRadius();

    TArray<AActor*> Overlapping;
    ScanSphere->GetOverlappingActors(Overlapping);

    for (AActor* A : Overlapping)
    {
        ApplyToActorTreasures(A, CurrentIntuitionFraction);
    }
}

bool UHiddenTreasureScannerComponent::IsLocal() const
{
    if (const APawn* P = Cast<APawn>(GetOwner()))
    {
        return P->IsLocallyControlled();
    }
    return true;
}


void UHiddenTreasureScannerComponent::UpdateRadius() const
{
    const float NewRadius = BaseRadius + MaxBonusRadius * CurrentIntuitionFraction;

    if (ScanSphere)
    {
        ScanSphere->SetSphereRadius(NewRadius, true);
    }
}

