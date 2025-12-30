// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UElectrocute : public UHAFBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
