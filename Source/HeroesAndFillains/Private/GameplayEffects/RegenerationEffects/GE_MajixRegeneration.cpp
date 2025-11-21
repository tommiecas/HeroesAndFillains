// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffects/RegenerationEffects/GE_MajixRegeneration.h"

#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_MajixRegeneration.h"

UGE_MajixRegeneration::UGE_MajixRegeneration()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//Tick every second
	Period.Value = 1.f;

	FGameplayEffectExecutionDefinition Exec;
	Exec.CalculationClass = UExecutionCalculation_MajixRegeneration::StaticClass();

	Executions.Add(Exec);

}