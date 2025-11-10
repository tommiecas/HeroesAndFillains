// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UHAFDamageGameplayAbility::CauseDamage(AActor* Target)
{
	if (!Target || !DamageEffectClass) return;

	// Source = the actor who owns this ability (usually player or enemy)
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// Target = the actor we’re damaging
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	// ✅ Skip if source and target are the same ASC (prevents self-damage)
	if (SourceASC == TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping self-damage attempt for %s"), *GetNameSafe(Target));
		return;
	}

	// Make outgoing spec
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	// Assign damage magnitudes
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	// ✅ Apply effect from source to target
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

FTaggedMontage UHAFDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages)
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection]; // ✅ Return the chosen montage!
	}

	// Return an empty struct if no montages exist
	return FTaggedMontage();
}
