// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HAFGameplayAbility.h"
#include "HAFProjectileSpell.generated.h"

/**
 * 
 */

class AHAFMajixProjectile;
class UGameplayEffect;

UCLASS()
class HEROESANDFILLAINS_API UHAFProjectileSpell : public UHAFGameplayAbility
{
	GENERATED_BODY()

public:
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffects")
	static FGameplayEffectContextHandle AddSourceObjectToContext(
		const FGameplayEffectContextHandle& Context, 
		UObject* SourceObject);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AHAFMajixProjectile> HAFMajixProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

};
