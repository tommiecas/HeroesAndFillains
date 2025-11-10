// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "ProjectileBullet.h"
#include "HAFRangedProjectileBullet.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AHAFRangedProjectileBullet : public AProjectileBullet
{
	GENERATED_BODY()

public:
	AHAFRangedProjectileBullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHAFRangedProjectileBullet> RangedProjectileBulletClass;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle BulletDamageEffectSpecHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> NewSphere;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass; // use this instead of a cached SpecHandle

	UPROPERTY()
	bool bHasAppliedEffect = false;
	
	UPROPERTY(VisibleAnywhere, Category="Audio|FX")
	UAudioComponent* LoopingSoundComponent = nullptr;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	bool bHit = false;

public:

};
