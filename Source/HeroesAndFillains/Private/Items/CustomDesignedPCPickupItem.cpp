 #include "Items/CustomDesignedPCPickupItem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/FillainCharacter.h"
#include "Components/ShapeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "HAFComponents/HiddenTreasureComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

// -------- Constructor --------
ACustomDesignedPCPickupItem::ACustomDesignedPCPickupItem()
{
    PrimaryActorTick.bCanEverTick = false;
    // NOTE: This class intentionally creates NO components in C++.
    // Designers add one UShapeComponent in BP and set AreaShape on the base.
}

// -------- lifecycle --------
void ACustomDesignedPCPickupItem::BeginPlay()
{
    Super::BeginPlay();

    // Bind our overlap handlers to the (single) AreaShape
    BindOverlapToAreaShape();
}

// -------- binding helpers --------
void ACustomDesignedPCPickupItem::BindOverlapToAreaShape()
{
    if (!AreaShape)
    {
        TArray<UShapeComponent*> Shapes; GetComponents(Shapes);
        if (Shapes.Num()) AreaShape = Shapes[0];
    }
    if (!AreaShape) return;

    AreaShape->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAreaBeginOverlap);
    AreaShape->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnAreaEndOverlap);
    AreaShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnAreaBeginOverlap);
    AreaShape->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnAreaEndOverlap);

    AreaShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AreaShape->SetGenerateOverlapEvents(true);
}

// -------- overlap handlers (component signature) --------
void ACustomDesignedPCPickupItem::OnAreaBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/,
                                                     AActor* OtherActor,
                                                     UPrimitiveComponent* /*OtherComp*/,
                                                     int32 /*OtherBodyIndex*/,
                                                     bool /*bFromSweep*/,
                                                     const FHitResult& /*SweepResult*/)
{
    if (!HasAuthority()) return;

    if (UAbilitySystemComponent* ASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
    {
        const FGameplayAttribute VisionAttr = UHAFAttributeSet::GetVisionAttribute();

        const float Cur  = ASC->GetNumericAttribute(VisionAttr);
        const float Base = ASC->GetNumericAttributeBase(VisionAttr);

        // Try to set base to +100
        ASC->SetNumericAttributeBase(VisionAttr, Base + 100.f);

        const float CurAfter  = ASC->GetNumericAttribute(VisionAttr);
        const float BaseAfter = ASC->GetNumericAttributeBase(VisionAttr);

        UE_LOG(LogTemp, Warning, TEXT("[BuffBox] Vision Cur=%.2f Base=%.2f  ->  CurAfter=%.2f BaseAfter=%.2f"),
            Cur, Base, CurAfter, BaseAfter);
    }
    
    if (OtherActor && OtherActor != this && HasAuthority())
    {
        OnOverlap(OtherActor); // the ONLY place that applies effects
    }
}

void ACustomDesignedPCPickupItem::OnAreaEndOverlap(UPrimitiveComponent* /*OverlappedComp*/,
                                                   AActor* OtherActor,
                                                   UPrimitiveComponent* /*OtherComp*/,
                                                   int32 /*OtherBodyIndex*/)
{
    if (!OtherActor || OtherActor == this) return;
    OnEndOverlap(OtherActor);
}

// -------- high-level overlap behavior (your original API) --------
void ACustomDesignedPCPickupItem::OnOverlap(AActor* TargetActor)
{
    UE_LOG(LogTemp, Warning, TEXT("[Pickup] Overlap by %s"), *GetNameSafe(TargetActor));

    if (bRequirePhysicalOverlapForApplication)
    {
        if (!AreaShape || !AreaShape->IsOverlappingActor(TargetActor))
        {
            UE_LOG(LogTemp, Verbose, TEXT("[Pickup] Skipped: no physical overlap with %s"), *GetNameSafe(TargetActor));
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[Pickup] Overlap by %s"), *GetNameSafe(TargetActor));



    // Optional UI ping you had before
    if (AFillainCharacter* FC = Cast<AFillainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        FC->OnMessageWidget.Broadcast(ActorLevel);
    }

    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }

    // If this pickup should vanish on overlap, you can Destroy() here,
    // or leave it to GE logic / BP (not destroying so duration/infinite can persist).
}

void ACustomDesignedPCPickupItem::OnEndOverlap(AActor* TargetActor)
{
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
    if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (!IsValid(TargetASC)) return;

        TArray<FActiveGameplayEffectHandle> HandlesToRemove;

        // ActiveEffectHandles is a TMap<FActiveGameplayEffectHandle, TWeakObjectPtr<UAbilitySystemComponent>>
        for (const auto& HandlePair : ActiveEffectHandles)
        {
            UAbilitySystemComponent* EffectOwnerASC = HandlePair.Value.Get();
            if (EffectOwnerASC == TargetASC)
            {
                TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
                HandlesToRemove.Add(HandlePair.Key);
            }
        }

        for (const FActiveGameplayEffectHandle& Handle : HandlesToRemove)
        {
            ActiveEffectHandles.FindAndRemoveChecked(Handle);
        }
    }
}

// -------- ASC/GE helpers --------
UAbilitySystemComponent* ACustomDesignedPCPickupItem::GetASCFromCharacter(AFillainCharacter* PlayerChar) const
{
    if (!PlayerChar) return nullptr;

    // Prefer the character's ASC component
    if (UAbilitySystemComponent* ASC = PlayerChar->FindComponentByClass<UAbilitySystemComponent>())
    {
        return ASC;
    }
    // Or if your character exposes a getter, use it:
    // return PlayerChar->GetAbilitySystemComponent();

    return nullptr;
}

void ACustomDesignedPCPickupItem::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    if (!GameplayEffectClass) return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (!TargetASC) return;

    UAbilitySystemComponent* SourceASC = TargetASC; // or your true source ASC
    FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
    Ctx.AddSourceObject(this);
    Ctx.AddInstigator(GetInstigator(), this);

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, Ctx);
    if (SpecHandle.IsValid())
    {
        SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    }
}

void ACustomDesignedPCPickupItem::SetHighlighted(bool bOn)
{
    for (UMeshComponent* M : HighlightMeshes)
    {
        if (!M) continue;
        // Outline
        M->SetRenderCustomDepth(bOn);
        M->SetCustomDepthStencilValue(1); // match your PP material
        // Emissive
        for (int32 i=0;i<M->GetNumMaterials();++i)
        {
            if (UMaterialInstanceDynamic* MID = M->CreateAndSetMaterialInstanceDynamic(i))
            {
                MID->SetScalarParameterValue(EmissiveParam, bOn ? 1.0f : 0.0f);
            }
        }
    }
}