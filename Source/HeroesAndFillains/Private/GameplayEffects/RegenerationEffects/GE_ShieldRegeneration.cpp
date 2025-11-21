// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffects/RegenerationEffects/GE_ShieldRegeneration.h"

#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_ShieldRegeneration.h"

UGE_ShieldRegeneration::UGE_ShieldRegeneration()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//Tick every second
	Period.Value = 1.f;

	FGameplayEffectExecutionDefinition Exec;
	Exec.CalculationClass = UExecutionCalculation_ShieldRegeneration::StaticClass();

	Executions.Add(Exec);

}