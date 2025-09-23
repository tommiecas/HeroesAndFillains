// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/HAFMajixProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "HAFGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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
	SetReplicateMovement(true);
	
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
}

void AHAFMajixProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		if (ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}


void AHAFMajixProjectile::OnNewSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();

		if (HasAuthority())
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				if (DamageEffectSpecHandle.IsValid())
				{
					// ✅ Extract the damage value from the spec (SetByCaller)
					float DamageAmount = 0.f;
					DamageEffectSpecHandle.Data->GetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(FName("SetByCaller.Damage")),
						false,
						DamageAmount
					);

					UE_LOG(LogTemp, Warning, TEXT("Fireball hit %s at %s | Damage: %.2f"),
						*OtherActor->GetName(),
						*SweepResult.ImpactPoint.ToString(),
						DamageAmount);
					
					// ✅ Apply damage effect to the target
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Projectile: Invalid DamageEffectSpecHandle on overlap!"));
				}
			}

			Destroy();
		}
		else
		{
			bHit = true;
		}
	}
}

