// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChainFleeHop.generated.h"

UCLASS()
class HEROESANDFILLAINS_API UBTTask_ChainFleeHop : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
