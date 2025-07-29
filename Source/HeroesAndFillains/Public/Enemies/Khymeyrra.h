// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Khymeyrra.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AKhymeyrra : public AEnemyBase
{
	GENERATED_BODY()

public:
	AKhymeyrra();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
