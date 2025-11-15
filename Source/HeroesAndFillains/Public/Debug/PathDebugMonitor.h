// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathDebugMonitor.generated.h"

UCLASS()
class HEROESANDFILLAINS_API APathDebugMonitor : public AActor
{
	GENERATED_BODY()

public:
	APathDebugMonitor();

protected:
	virtual void BeginPlay() override;

private:
	void CheckPathComponents();

	FTimerHandle PathTickMonitorHandle;

};