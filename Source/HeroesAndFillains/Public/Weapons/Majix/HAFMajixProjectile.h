// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Majix/MajixWeapon.h"
#include "HAFMajixProjectile.generated.h"

class UHAFProjectileSpell;
class UNiagaraComponent;
class USphereComponent;
class UProjectileMovementComponent;
class AHAFMajixProjectile;

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API AHAFMajixProjectile : public AMajixWeapon
{
	GENERATED_BODY()
	
public:	
	AHAFMajixProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHAFMajixProjectile> FireBoltProjectileClass;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	bool IsValidOverlap(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Component")
	UNiagaraSystem* MajixWeaponMesh;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnHit();
	
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnNewSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	

	bool bHit = false;

	UPROPERTY(VisibleAnywhere, Category="Audio|FX")
	UAudioComponent* LoopingSoundComponent = nullptr;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

private:
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UHAFProjectileSpell> HAFProjectileSpellClass;
	
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass; // use this instead of a cached SpecHandle

	UPROPERTY()
	bool bHasAppliedEffect = false;

public:
	FORCEINLINE TSubclassOf<UHAFProjectileSpell> GetHAFProjectileSpellClass() const { return HAFProjectileSpellClass; }


};
