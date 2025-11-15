// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/HitScanWeaponAmmo.h"
#include "Weapons/Ranged/ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Ranged/Projectile.h"
#include "Sound/SoundCue.h"

#include "GameFramework/Character.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Ranged/Projectile.h"
#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Weapons/Ranged/RocketMovementComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Public/NiagaraSystemInstance.h"


AHitScanWeaponAmmo::AHitScanWeaponAmmo()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AHitScanWeaponAmmo::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}



void AHitScanWeaponAmmo::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	
	// Damage is handled through ExplodeDamage() which should use GAS
	ExplodeDamage();
	StartDestroyTimer();
}

void AHitScanWeaponAmmo::Destroyed()
{

}




