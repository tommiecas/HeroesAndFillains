#include "GameMode/HAFHybridGameMode.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/HAFGameInstance.h"
#include "GameMode/HAFSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModels/MVVM_LoadSlot.h"
#include "GameMode/HAFSaveGame.h"
#include "HeroesAndFillains/HAFLogChannels.h"
#include "Interfaces/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

AHAFHybridGameMode::AHAFHybridGameMode()
{
    CurrentMode = EGameModeType::EGMT_PvE; // match your enum definition
}

void AHAFHybridGameMode::SetGameMode(EGameModeType NewMode)
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

void AHAFHybridGameMode::SaveSlotData(UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const
{
    if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
    {
        UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
    }
    USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    UHAFSaveGame* LoadScreenSaveGame = Cast<UHAFSaveGame>(SaveGameObject);
    LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
    LoadScreenSaveGame->SaveSlotStatus = Taken;
    LoadScreenSaveGame->LevelName = LoadSlot->GetLevelName();
    LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
    
    UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

UHAFSaveGame* AHAFHybridGameMode::RetrieveInGameSaveData()
{
    UHAFGameInstance* HAFGameInstance = Cast<UHAFGameInstance>(GetGameInstance());

    const FString InGameLoadSlotName = HAFGameInstance->LoadSlotName;
    const int32 InGameLoadSlotIndex = HAFGameInstance->LoadSlotIndex;

    return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AHAFHybridGameMode::SaveInGameProgressData(UHAFSaveGame* SaveObject)
{
    UHAFGameInstance* HAFGameInstance = Cast<UHAFGameInstance>(GetGameInstance());

    const FString InGameLoadSlotName = HAFGameInstance->LoadSlotName;
    const int32 InGameLoadSlotIndex = HAFGameInstance->LoadSlotIndex;
    HAFGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;

    UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AHAFHybridGameMode::SaveWorldState(UWorld* World) const
{
    FString WorldName = World->GetMapName();
    WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

    UHAFGameInstance* HAFGI = Cast<UHAFGameInstance>(GetGameInstance());
    check(HAFGI);

    if (UHAFSaveGame* SaveGame = GetSaveSlotData(HAFGI->LoadSlotName, HAFGI->LoadSlotIndex))
    {
        if (!SaveGame->HasLevel(WorldName))
        {
            FSavedLevel NewSavedLevel;
            NewSavedLevel.LevelAssetName = WorldName;
            SaveGame->SavedLevels.Add(NewSavedLevel);
        }
        FSavedLevel SavedLevel = SaveGame->GetSavedLevelWithLevelName(WorldName);
        SavedLevel.SavedActors.Empty();

        for (FActorIterator It(World); It; ++It)
        {
            AActor* Actor = *It;

            if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

            FSavedActor SavedActor;
            SavedActor.ActorName = Actor->GetFName();
            SavedActor.ActorTransform = Actor->GetTransform();

            FMemoryWriter MemoryWriter(SavedActor.Bytes);

            FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
            Archive.ArIsSaveGame = true;
            Actor->Serialize(Archive);
            SavedLevel.SavedActors.AddUnique(SavedActor);
        }

        for (FSavedLevel& LevelToReplace : SaveGame->SavedLevels)
        {
            if (LevelToReplace.LevelAssetName == WorldName)
            {
                LevelToReplace = SavedLevel;
            }
        }

        UGameplayStatics::SaveGameToSlot(SaveGame, HAFGI->LoadSlotName, HAFGI->LoadSlotIndex);
    }
}
 
void AHAFHybridGameMode::LoadWorldState(UWorld* World) const
{
    FString WorldName = World->GetMapName();
    WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

    UHAFGameInstance* HAFGI = Cast<UHAFGameInstance>(GetGameInstance());
    check(HAFGI);

    if (UGameplayStatics::DoesSaveGameExist(HAFGI->LoadSlotName, HAFGI->LoadSlotIndex))
    {
        UHAFSaveGame* SaveGame = Cast<UHAFSaveGame>(UGameplayStatics::LoadGameFromSlot(HAFGI->LoadSlotName, HAFGI->LoadSlotIndex));
        if (SaveGame == nullptr)
        {
            UE_LOG(LogHAF, Error, TEXT("Failed to load slot"));
            return;
        }
        
        for (FActorIterator It(World); It; ++It)
        {
            AActor* Actor = *It;

            if (!Actor->Implements<USaveInterface>()) continue;

            for (FSavedActor SavedActor : SaveGame->GetSavedLevelWithLevelName(WorldName).SavedActors)
            {
                if (SavedActor.ActorName == Actor->GetFName())
                {
                    if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
                    {
                        Actor->SetActorTransform(SavedActor.ActorTransform);
                    }

                    FMemoryReader MemoryReader(SavedActor.Bytes);

                    FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
                    Archive.ArIsSaveGame = true;
                    Actor->Serialize(Archive); //converts binary bytes back to variables

                    ISaveInterface::Execute_LoadActor(Actor);
                }
            }
        }
    }
}



void AHAFHybridGameMode::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
    {
        UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
    }
}

UHAFSaveGame* AHAFHybridGameMode::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
    USaveGame* SaveGameObject = nullptr;
    if (UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex))
    {
        SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
    }
    else
    {
        SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    }
    UHAFSaveGame* LoadScreenSaveGame = Cast<UHAFSaveGame>(SaveGameObject);
    return LoadScreenSaveGame;
}

void AHAFHybridGameMode::TravelToMap(UMVVM_LoadSlot* Slot)
{
    const FString SlotName = Slot->GetLoadSlotName();
    const int32 SlotIndex = Slot->SlotIndex;

    UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, GameLevels.FindChecked(Slot->GetLevelName()));
}

AActor* AHAFHybridGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    Super:: ChoosePlayerStart_Implementation(Player);
    if (GetGameInstance() == nullptr) return nullptr;
    if (GetGameInstance() != nullptr)
    {
        UHAFGameInstance* HAFGameInstance = Cast<UHAFGameInstance>(GetGameInstance());
        TArray<AActor*> Actors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
        if (HAFGameInstance && Actors.Num() > 0)
        {
            AActor* SelectedActor = Actors[0];
            for (AActor* Actor : Actors)
            {
                if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
                {
                    if (PlayerStart->PlayerStartTag == HAFGameInstance->PlayerStartTag)
                    {
                        SelectedActor = PlayerStart;
                        break;
                    }
                }
            }
            return SelectedActor;
        }
    }
    return nullptr;
}

void AHAFHybridGameMode::OnEnterPvE()
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

void AHAFHybridGameMode::OnEnterPvP()
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
 * AHAFHybridGameMode* GM = GetWorld()->GetAuthGameMode<AHAFHybridGameMode>();
 * if (GM) GM->SetGameMode(EGameModeType::PvP);
 */
