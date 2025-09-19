// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Majix/MajixWeapon.h"
#include "HAFProjectile.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UProjectileMovementComponent;
class AHAFProjectile;

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API AHAFProjectile : public AMajixWeapon
{
	GENERATED_BODY()
	
public:	
	AHAFProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass; // use this instead of a cached SpecHandle

	UPROPERTY()
	bool bHasAppliedEffect = false;
	
	UPROPERTY(VisibleAnywhere, Category="Audio|FX")
	UAudioComponent* LoopingSoundComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent>  ProjectileRoot;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	bool bHit = false;

};
