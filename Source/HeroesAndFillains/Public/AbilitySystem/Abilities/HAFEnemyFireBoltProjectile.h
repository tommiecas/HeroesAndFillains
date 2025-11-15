// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFDamageGameplayAbility.h"
#include "HAFEnemyFireBoltProjectile.generated.h"

class AHAFMajixProjectile;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFEnemyFireBoltProjectile : public UHAFDamageGameplayAbility
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
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnEnemyFireBoltProjectile(const FVector& ProjectileTargetLocation);


};

