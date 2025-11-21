// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffects/RegenerationEffects/GE_HealthRegeneration.h"

#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_HealthRegeneration.h"

UGE_HealthRegeneration::UGE_HealthRegeneration()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//Tick every second
	Period.Value = 1.f;

	FGameplayEffectExecutionDefinition Exec;
	Exec.CalculationClass = UExecutionCalculation_HealthRegeneration::StaticClass();

	Executions.Add(Exec);

}