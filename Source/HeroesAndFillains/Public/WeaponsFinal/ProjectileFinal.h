// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileFinal.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AProjectileFinal : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileFinal();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

};
