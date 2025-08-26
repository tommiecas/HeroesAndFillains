#include "Items/PCPickupBaseItem.h"
#include "Components/ShapeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Items/CustomDesignedPCPickupItem.h"

APCPickupBaseItem::APCPickupBaseItem()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APCPickupBaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
}

void APCPickupBaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    
}

void APCPickupBaseItem::EnableCustomDepth(bool bEnable)
{
    
}

void APCPickupBaseItem::ShowPickupAndInfoWidgets(bool bShow)
{
    
}

float APCPickupBaseItem::TransformedSin()
{
    return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float APCPickupBaseItem::TransformedCos()
{
    return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void APCPickupBaseItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ResolveAreaShapeByTag(); // keeps it live in-editor when you tweak components/tags
}

void APCPickupBaseItem::BeginPlay()
{
    Super::BeginPlay();
    ResolveAreaShapeByTag(); // ensures it’s set at runtime

    if (!AreaShape)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: AreaShape not set or found by tag '%s'."),
               *GetName(), *AreaShapeTag.ToString());
        return;
    }

    // Safe overlap bindings
    if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(AreaShape))
    {
        Prim->SetGenerateOverlapEvents(true);
        Prim->OnComponentBeginOverlap.AddDynamic(this, &APCPickupBaseItem::OnAreaBegin);
        Prim->OnComponentEndOverlap.AddDynamic(this, &APCPickupBaseItem::OnAreaEnd);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: AreaShape is not a UPrimitiveComponent (got %s)."),
               *GetName(), *GetNameSafe(AreaShape));
    }
}

void APCPickupBaseItem::ResolveAreaShapeByTag()
{
    // If already assigned and still valid, keep it
    if (AreaShape && AreaShape->IsRegistered()) return;

    AreaShape = nullptr;

    if (!AreaShapeTag.IsNone())
    {
        TArray<UActorComponent*> Hits = GetComponentsByTag(UShapeComponent::StaticClass(), AreaShapeTag);
        if (Hits.Num() > 0)
        {
            AreaShape = Cast<UShapeComponent>(Hits[0]);
        }
    }

    if (!AreaShape)
    {
        // Fallback: first shape component on the actor (optional)
        if (UActorComponent* C = GetComponentByClass(UShapeComponent::StaticClass()))
        {
            AreaShape = Cast<UShapeComponent>(C);
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("%s: ResolveAreaShapeByTag -> %s"),
           *GetName(), *GetNameSafe(AreaShape));
}

void APCPickupBaseItem::OnAreaBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
                            int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    
    UE_LOG(LogTemp, Log, TEXT("%s: Area Begin Overlap with %s"), *GetName(), *GetNameSafe(Other));
}

void APCPickupBaseItem::OnAreaEnd(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
   
    UE_LOG(LogTemp, Log, TEXT("%s: Area End Overlap with %s"), *GetName(), *GetNameSafe(Other));
}
