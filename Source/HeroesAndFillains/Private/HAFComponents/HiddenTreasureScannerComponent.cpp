#include "HAFComponents/HiddenTreasureScannerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "HAFComponents/HiddenTreasureComponent.h"
#include "DrawDebugHelpers.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

UHiddenTreasureScannerComponent::UHiddenTreasureScannerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHiddenTreasureScannerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (const APawn* P = Cast<APawn>(GetOwner()))
    {
        if (!P->IsLocallyControlled())
        {
            // Don’t run on server or non-local clients
            if (ScanSphere) { ScanSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); }
            return;
        }
    }
    UpdateRadius();
    TArray<AActor*> Overlapping; ScanSphere->GetOverlappingActors(Overlapping);
    for (AActor* A : Overlapping) { ApplyToActorTreasures(A, CurrentIntuitionFraction); }
}

void UHiddenTreasureScannerComponent::OnScanBegin(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsLocal()) return;                    // client-only visuals
    ApplyToActorTreasures(OtherActor, CurrentIntuitionFraction);  // ✅ glow only
}

void UHiddenTreasureScannerComponent::OnScanEnd(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!IsLocal()) return;                    // client-only visuals
    ApplyToActorTreasures(OtherActor, 0.f);    // ✅ remove glow
}

bool UHiddenTreasureScannerComponent::IsLocal() const
{
    if (const APawn* P = Cast<APawn>(GetOwner())) return P->IsLocallyControlled();
    return true; // non-pawn owners just run
}

void UHiddenTreasureScannerComponent::SetIntuitionFraction(float Fraction)
{
    CurrentIntuitionFraction = FMath::Clamp(Fraction, 0.f, 1.f);
    if (!ScanSphere || !ScanSphere->IsRegistered()
        || ScanSphere->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
    {
        return;
    }
    UpdateRadius();
    TArray<AActor*> Overlapping;
    ScanSphere->GetOverlappingActors(Overlapping);
    for (AActor* A : Overlapping) { ApplyToActorTreasures(A, CurrentIntuitionFraction); }
}

void UHiddenTreasureScannerComponent::OnRegister()
{
    Super::OnRegister();

    if (!ScanSphere)
    {
        ScanSphere = NewObject<USphereComponent>(GetOwner(), TEXT("IntuitionScanSphere"));
        ScanSphere->SetupAttachment(GetOwner()->GetRootComponent());
        ScanSphere->RegisterComponent(); // ensures it's valid before BeginPlay
    }

    // Collision wiring once
    ScanSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ScanSphere->SetCollisionObjectType(ECC_WorldDynamic);
    ScanSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    ScanSphere->SetCollisionResponseToChannel(ECC_Treasure, ECR_Overlap);
    ScanSphere->SetGenerateOverlapEvents(true);

    // Bind overlaps once
    ScanSphere->OnComponentBeginOverlap.AddDynamic(this, &UHiddenTreasureScannerComponent::OnScanBegin);
    ScanSphere->OnComponentEndOverlap  .AddDynamic(this, &UHiddenTreasureScannerComponent::OnScanEnd);
}

void UHiddenTreasureScannerComponent::UpdateRadius() const
{
    const float NewRadius = BaseRadius + MaxBonusRadius * CurrentIntuitionFraction;
    if (ScanSphere) ScanSphere->SetSphereRadius(NewRadius, true);
#if !(UE_BUILD_SHIPPING)
    // DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), NewRadius, 16, FColor::Yellow, false, 0.05f);
#endif
}

void UHiddenTreasureScannerComponent::ApplyToActorTreasures(AActor* Other, float Fraction)
{
    if (!Other || Other == GetOwner()) return;
    for (auto* HT : TInlineComponentArray<UHiddenTreasureComponent*>(Other, true))
    {
        HT->ApplyIntuitionScale(Fraction); // drives emissive on the treasure
    }
}