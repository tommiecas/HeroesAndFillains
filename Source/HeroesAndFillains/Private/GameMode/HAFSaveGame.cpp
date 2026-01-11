// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/HAFSaveGame.h"

FSavedLevel UHAFSaveGame::GetSavedLevelWithLevelName(const FString& NameOfLevel)
{
	for (const FSavedLevel& Level : SavedLevels)
	{
		if (Level.LevelAssetName == NameOfLevel)
		{
			return Level;
		}
	}
	return FSavedLevel();
}

bool UHAFSaveGame::HasLevel(const FString& MapName)
{
	for (const FSavedLevel& Level : SavedLevels)
	{
		if (Level.LevelAssetName == MapName)
		{
			return true;
		}
	}
	return false;
}
