// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "NiagaraPassiveSpellComponent.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UNiagaraPassiveSpellComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UNiagaraPassiveSpellComponent();
	
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
};
