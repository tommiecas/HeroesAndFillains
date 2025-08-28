// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HAFGameplayAbility.h"
#include "HAFProjectileSpell.generated.h"

class AHAFProjectile;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFProjectileSpell : public UHAFGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AHAFProjectile> ProjectileClass;
};
