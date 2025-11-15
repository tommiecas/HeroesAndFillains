// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/HitInterface.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Damage is now handled through GAS via GetHit_Implementation
	// which will apply the appropriate GameplayEffect
	if (IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor))
	{
		HitInterface->Execute_GetHit(OtherActor, Hit.ImpactPoint, GetOwner());
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

