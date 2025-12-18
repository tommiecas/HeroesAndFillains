#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    MyObj->ReadyForActivation();   // ✅ Activate immediately so it’s properly registered
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

    if (!AbilitySystemComponent.IsValid() || !PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ AbilitySystem or PC invalid in TargetDataUnderMouse::Activate"));
        EndTask();
        return;
    }

    // --- Initial cursor trace (one-shot broadcast when task starts) ---
    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

    if (!CursorHit.bBlockingHit)
    {
        // Ray forward from camera 10,000 units
        FVector WorldOrigin, WorldDir;
        PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDir);

        CursorHit.Location = WorldOrigin + WorldDir * 10000.f;
    }
    
    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHit;
    DataHandle.Add(Data);

    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        GetActivationPredictionKey(),
        DataHandle,
        FGameplayTag(),
        AbilitySystemComponent->ScopedPredictionKey);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ TargetDataUnderMouse broadcasting ValidData now"));

    // ✅ Keep task alive — do NOT EndTask() here
    // Optionally start a repeating hover update
    const float HoverUpdateRate = 0.05f; // 20 Hz
    GetWorld()->GetTimerManager().SetTimer(
        HoverTimerHandle,
        this,
        &UTargetDataUnderMouse::SendMouseCursorData,
        HoverUpdateRate,
        true);

    UE_LOG(LogTemp, Error, TEXT("CURSOR HIT: Actor=%s  Location=%s"),
    CursorHit.GetActor() ? *CursorHit.GetActor()->GetName() : TEXT("None"),
    *CursorHit.Location.ToString());

    MouseHitLocation = CursorHit.Location;
    MouseHitActor = CursorHit.GetActor();
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
    if (!AbilitySystemComponent.IsValid()) return;

    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    if (!PC) return;

    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECC_Target, true, CursorHit);
    AEnemyBase* HitEnemy = Cast<AEnemyBase>(CursorHit.GetActor());

    // Handle hover highlighting
    if (HitEnemy != LastHoveredEnemy)
    {
        if (LastHoveredEnemy)
        {
            LastHoveredEnemy->OnHoverEnd();
            LastHoveredEnemy->UnHighlightActor();
        }

        if (HitEnemy)
        {
            HitEnemy->OnHoverStart();
            HitEnemy->HighlightActor();
        }

        LastHoveredEnemy = HitEnemy;
    }

    // Send updated target data each tick if you want continuous aiming
    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHit;
    DataHandle.Add(Data);

    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        GetActivationPredictionKey(),
        DataHandle,
        FGameplayTag(),
        AbilitySystemComponent->ScopedPredictionKey);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
    UE_LOG(LogTemp, Error, TEXT("CURSOR HIT: Actor=%s  Location=%s"),
    CursorHit.GetActor() ? *CursorHit.GetActor()->GetName() : TEXT("None"),
    *CursorHit.Location.ToString());
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}

void UTargetDataUnderMouse::OnDestroy(bool bInOwnerFinished)
{
    GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);

    if (LastHoveredEnemy)
    {
        LastHoveredEnemy->OnHoverEnd();
        LastHoveredEnemy->UnHighlightActor();
        LastHoveredEnemy = nullptr;
    }

    UE_LOG(LogTemp, Warning, TEXT("🧹 TargetDataUnderMouse cleaned up in OnDestroy"));

    Super::OnDestroy(bInOwnerFinished);
}
