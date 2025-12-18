// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/HAFMajixProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "HAFGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Kismet/GameplayStatics.h"

AHAFMajixProjectile::AHAFMajixProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	// 2) Reattach & neutralize the base pickup sphere so it’s harmless here
	// IMPORTANT: Do NOT destroy default subobjects in a constructor (CDO runs here).
	// Disable / hide instead to avoid engine linker/GC invariants breaking.
	if (AreaSphere)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetGenerateOverlapEvents(false);
		AreaSphere->SetComponentTickEnabled(false);
		AreaSphere->SetHiddenInGame(true);
		AreaSphere->Deactivate();
	}
	if (HoverDecal)
	{
		HoverDecal->SetHiddenInGame(true);
		HoverDecal->SetVisibility(false, true);
		HoverDecal->Deactivate();
	}
	if (HoverLight)
	{
		HoverLight->SetVisibility(false, true);
		HoverLight->SetComponentTickEnabled(false);
		HoverLight->Deactivate();
	}
	if (PickupGearWidgetComponent)
	{
		PickupGearWidgetComponent->SetVisibleFlag(false);
		PickupGearWidgetComponent->SetVisibility(false, true);
		PickupGearWidgetComponent->SetComponentTickEnabled(false);
		PickupGearWidgetComponent->Deactivate();
	}
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetVisibleFlag(false);
		ItemInfoWidgetComponent->SetVisibility(false, true);
		ItemInfoWidgetComponent->SetComponentTickEnabled(false);
		ItemInfoWidgetComponent->Deactivate();
	}
	if (WeaponMesh)
	{
		WeaponMesh->SetVisibility(false, true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetComponentTickEnabled(false);
		WeaponMesh->Deactivate();
	}
	
	NewSphere = CreateDefaultSubobject<USphereComponent>(TEXT("NewSphere"));
	SetRootComponent(NewSphere);
	NewSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NewSphere->SetCollisionObjectType(ECC_Projectile);
	NewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	NewSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	NewSphere->IgnoreActorWhenMoving(this, true);
	
	// Projectile movement pushes the sphere (root)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	if (WeaponBox)
	{
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponBox->SetGenerateOverlapEvents(false);
		WeaponBox->SetHiddenInGame(true);
		WeaponBox->Deactivate();
	}

	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s"), *GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile rotation: %s"), *GetActorRotation().ToString());
}

void AHAFMajixProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	if (ensureMsgf(NewSphere != nullptr, TEXT("NewSphere was not created")))
	{
		NewSphere->OnComponentBeginOverlap.AddDynamic(this, &AHAFMajixProjectile::OnNewSphereOverlap);
	}

	if (AActor* MyOwner = GetOwner())
	{
		NewSphere->IgnoreActorWhenMoving(MyOwner, true);
	}
	
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s"), *GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile rotation: %s"), *GetActorRotation().ToString());
	if (LoopingSound && LoopingSoundComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Looping Sound of %s"), *LoopingSound->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Looping Sound Component of %s"), *LoopingSoundComponent->GetName());
	}
	if (ImpactEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Impact Effect of %s"), *ImpactEffect->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s with velocity %s"), *GetActorLocation().ToString(), *ProjectileMovement->Velocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile BeginPlay Location: %s, Velocity: %s"), *GetActorLocation().ToString(), *ProjectileMovement->Velocity.ToString());
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + ProjectileMovement->Velocity * 0.1f, FColor::Green, false, 3.0f, 0, 5.0f);
  

}

void AHAFMajixProjectile::OnHit()
{
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	if (ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AHAFMajixProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if (!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}


void AHAFMajixProjectile::OnNewSphereOverlap(
    UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return;
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) return;

	if (!UHAFAbilitySystemBlueprintLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return;
	if (!bHit) OnHit();
	if (!bHit && OtherActor && OtherActor != this && OtherActor != GetOwner()) OnHit();
	// UE_LOG(LogTemp, Warning, TEXT("🔥 Projectile overlap! Hit: %s"), *GetNameSafe(OtherActor));
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// UE_LOG(LogTemp, Warning, TEXT("✅ Target %s has ASC"), *GetNameSafe(OtherActor));
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;

				const FVector KnockbackDirection = Rotation.Vector();
				const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
				DamageEffectParams.KnockbackForce = KnockbackForce;
			}

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UHAFAbilitySystemBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
    }
	else bHit = true;
}

