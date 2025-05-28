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
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	BulletMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
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
		CollisionBox->IgnoreActorWhenMoving(Owner, true);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,  FVector NormalImpulse, const FHitResult& Hit)
{
	HitResult = Hit.ImpactPoint;
	HitRotation = Hit.ImpactNormal.Rotation();

	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		FillainCharacter->MulticastHit();
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



