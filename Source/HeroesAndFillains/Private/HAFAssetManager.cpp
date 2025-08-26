// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFAssetManager.h"
#include "HAFGameplayTags.h"

UHAFAssetManager& UHAFAssetManager::Get()
{
	check(GEngine);
	
	UHAFAssetManager* HAFAssetManager = Cast<UHAFAssetManager>(GEngine->AssetManager);
	return *HAFAssetManager;
}

void UHAFAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FHAFGameplayTags::InitializeNativeGameplayTags();	
}
