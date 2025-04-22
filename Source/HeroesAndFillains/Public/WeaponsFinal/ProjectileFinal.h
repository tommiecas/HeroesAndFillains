// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileFinal.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AProjectileFinal : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectileFinal();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;


	
	FVector HitResult;
	FRotator HitRotation;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Tracer;

	class UNiagaraComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};
