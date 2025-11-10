// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFProjectileSpell.h"
#include "HAFProjectileFiringAbility.generated.h"

class AHAFRangedProjectileBullet;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFProjectileFiringAbility : public UHAFDamageGameplayAbility
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
	TSubclassOf<AHAFRangedProjectileBullet> HAFRangedProjectileBulletClass;
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileBullet(const FVector& ProjectileTargetLocation);


};
