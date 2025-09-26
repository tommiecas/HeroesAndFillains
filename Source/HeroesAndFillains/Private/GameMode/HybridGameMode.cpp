#include "GameMode/HybridGameMode.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

AHybridGameMode::AHybridGameMode()
{
    CurrentMode = EGameModeType::EGMT_PvE; // match your enum definition
}

void AHybridGameMode::SetGameMode(EGameModeType NewMode)
{
    if (CurrentMode == NewMode) return;

    CurrentMode = NewMode;

    switch (CurrentMode)
    {
    case EGameModeType::EGMT_PvE:
        OnEnterPvE();
        break;

    case EGameModeType::EGMT_PvP:
        OnEnterPvP();
        break;
    }
}

void AHybridGameMode::OnEnterPvE()
{
    Super::OnEnterPvE(); // ✅ safe because base implements an empty version

    UE_LOG(LogTemp, Log, TEXT("Switched to PvE mode"));

    // Example: disable PvP rules (friendly fire off, etc.)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AController* PC = It->Get())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
                {
                    ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayState.PvP"));
                    ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayState.PvE"));
                }
            }
        }
    }
}

void AHybridGameMode::OnEnterPvP()
{
    Super::OnEnterPvP(); // ✅ safe because base implements an empty version

    UE_LOG(LogTemp, Log, TEXT("Switched to PvP mode"));

    // Example: enable PvP rules (friendly fire on, scoring enabled, etc.)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AController* PC = It->Get())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
                {
                    ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayState.PvE"));
                    ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("GameplayState.PvP"));
                }
            }
        }
    }
}

/*
 * When you want to switch modes, call:
 * AHybridGameMode* GM = GetWorld()->GetAuthGameMode<AHybridGameMode>();
 * if (GM) GM->SetGameMode(EGameModeType::PvP);
 */
