// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyInfo.h"

FEnemyWiki UEnemyInfo::GetEnemyInfo(EEnemyType EnType)
{
	if (const FEnemyWiki* EnemyWiki = EnemyInformation.Find(EnType))
	{
		return *EnemyWiki;
	}

	UE_LOG(LogTemp, Warning, TEXT("⚠️ EnemyInfo: Missing entry for class %d"), static_cast<int32>(EnType));
	return FEnemyWiki(); // return empty, safe default
}