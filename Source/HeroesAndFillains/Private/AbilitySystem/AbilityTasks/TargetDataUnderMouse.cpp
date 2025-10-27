#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
    return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    if (bIsLocallyControlled)
    {
        // Start continuous hover checks every 0.05s (~20 FPS)
        GetWorld()->GetTimerManager().SetTimer(HoverTimerHandle, this, &UTargetDataUnderMouse::SendMouseCursorData, 0.05f, true);
    }
    else
    {
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
            .AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
        const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
        if (!bCalledDelegate)
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
    if (!AbilitySystemComponent.IsValid())
        return;

    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    if (!PC) return;

    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
    AEnemyBase* HitEnemy = Cast<AEnemyBase>(CursorHit.GetActor());

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

    // --- TargetData logic for your ability (still valid) ---
    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHit;
    DataHandle.Add(Data);

    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        GetActivationPredictionKey(),
        DataHandle,
        FGameplayTag(),
        AbilitySystemComponent->ScopedPredictionKey
    );

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
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

    Super::OnDestroy(bInOwnerFinished);
}
