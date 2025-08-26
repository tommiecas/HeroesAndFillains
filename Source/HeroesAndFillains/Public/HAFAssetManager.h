// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HAFAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UHAFAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
	
};
