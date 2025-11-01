// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HAFAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AHAFAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AHAFAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
};
