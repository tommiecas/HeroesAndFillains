// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharClass)
{
    if (const FCharacterClassDefaultInfo* FoundInfo = CharacterClassInformation.Find(CharClass))
    {
        return *FoundInfo;
    }

    UE_LOG(LogTemp, Warning, TEXT("⚠️ CharacterClassInfo: Missing entry for class %d"), static_cast<int32>(CharClass));
    return FCharacterClassDefaultInfo(); // return empty, safe default
}