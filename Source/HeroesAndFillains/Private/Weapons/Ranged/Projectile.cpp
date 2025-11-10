// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Ranged/Projectile.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/FillainCharacter.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox); // ✅ now the projectile movement moves the collision box
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);

	// then attach the mesh to the box
	BulletMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(CollisionBox);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	if (ImpactFX) ImpactFX->SetAutoDestroy(true);
	Super::Destroyed();


}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Tracer, CollisionBox->GetComponentLocation(), CollisionBox->GetComponentRotation());
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,  FVector NormalImpulse, const FHitResult& Hit)
{
	if (ImpactNiagaraSystem)
	{
		ImpactFX = UNiagaraFunctionLibrary::SpawnSystemAttached(ImpactNiagaraSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}

void AProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(GetOwner());
			
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, // World context object
				Damage, // BaseDamage
				10.f, // MinimumDamage
				GetActorLocation(), // Origin
				DamageInnerRadius, // DamageInnerRadius
				DamageOuterRadius, // DamageOuterRadius
				1.f, // DamageFalloff
				UDamageType::StaticClass(), // DamageTypeClass
				TArray<AActor*>(), // IgnoreActors
				this, // DamageCauser
				FiringController // InstigatorController
			);
		}
	}
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime
	);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}



