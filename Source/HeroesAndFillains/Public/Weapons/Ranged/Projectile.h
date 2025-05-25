// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	/*******************************************
	****    Used with Server Side Rewind    ****
	*******************************************/

	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	FVector HitResult;
	FRotator HitRotation;

protected:
	virtual void BeginPlay() override;

	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 300.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 600.f;


private:	
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Tracer;

	class UNiagaraComponent* TracerComponent;

	FTimerHandle DestroyTimer;

	UPROPERTY()
	ERangedType RangedType;;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:
	FORCEINLINE ERangedType GetRangedType() const{ return RangedType; }
	FORCEINLINE float GetDamage() const { return Damage; }
	
};
