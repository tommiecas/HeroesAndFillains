// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "HAFGameplayTags.h"
#include "Navigation/PathFollowingComponent.h"

void UHAFDamageGameplayAbility::CauseDamage(AActor* Target)
{
	if (!Target || !DamageEffectClass) return;

	// Source ASC
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// Target ASC
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	// Prevent self-damage
	if (SourceASC == TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping self-damage attempt for %s"), *GetNameSafe(Target));
		return;
	}

	// Make outgoing GE spec
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	// Set By Caller magnitudes
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	// ------------------------------------------------------------
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

float UHAFDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType)
{
	checkf(DamageTypes.Contains(DamageType), TEXT("GameplayAbility %s does not contain DamageType %s"), *GetNameSafe(this), *DamageType.ToString());
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}
