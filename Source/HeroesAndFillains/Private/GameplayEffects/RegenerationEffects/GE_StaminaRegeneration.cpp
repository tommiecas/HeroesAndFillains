// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffects/RegenerationEffects/GE_StaminaRegeneration.h"

#include "AbilitySystem/ExecutionCalculations/Regeneration/ExecutionCalculation_StaminaRegeneration.h"

UGE_StaminaRegeneration::UGE_StaminaRegeneration()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//Tick every second
	Period.Value = 1.f;

	FGameplayEffectExecutionDefinition Exec;
	Exec.CalculationClass = UExecutionCalculation_StaminaRegeneration::StaticClass();

	Executions.Add(Exec);

}