// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BehaviorTree/BTService.h"
#include "BTS_SelectCharmTarget.generated.h"

UCLASS()
class HEROESANDFILLAINS_API UBTS_SelectCharmTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_SelectCharmTarget();

protected:
	UPROPERTY(EditAnywhere, Category="Charm")
	float SearchRadius = 4000.f;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
