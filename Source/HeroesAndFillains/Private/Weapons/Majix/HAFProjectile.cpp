// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/HAFProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Kismet/GameplayStatics.h"

AHAFProjectile::AHAFProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	ProjectileRoot = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileRoot"));
	ProjectileRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(ProjectileRoot);
	ProjectileRoot->InitSphereRadius(100.f); 
	ProjectileRoot->SetCollisionObjectType(ECC_Projectile);
	ProjectileRoot->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileRoot->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileRoot->SetCollisionResponseToChannel(ECC_WorldStatic,  ECR_Overlap);
	ProjectileRoot->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ProjectileRoot->SetCollisionResponseToChannel(ECC_Visibility,   ECR_Block);
	ProjectileRoot->SetCollisionResponseToChannel(ECC_Enemy,ECR_Overlap);
	ProjectileRoot->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	// 2) Reattach & neutralize the base pickup sphere so it’s harmless here
	if (AreaSphere) // inherited from AWeaponBase
	{
		AreaSphere->SetupAttachment(ProjectileRoot);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetGenerateOverlapEvents(false);
		AreaSphere->SetHiddenInGame(true);
		AreaSphere->Deactivate(); // stops component tick/activation if any
		AreaSphere->SetCanEverAffectNavigation(false);
	}
	
	// Inherited mesh from AWeaponBase still exists — just neutralize it
	if (WeaponMesh) // whatever your base calls it (e.g., WeaponMesh or Mesh)
	{
		WeaponMesh->SetupAttachment(ProjectileRoot);                  // no longer root
		WeaponMesh->SetHiddenInGame(true);                        // we don't want to see it
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetGenerateOverlapEvents(false);
	}
	
	// Projectile movement pushes the sphere (root)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement Component"));
	ProjectileMovement->UpdatedComponent = ProjectileRoot;     // <— critical
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AHAFProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	ProjectileRoot->OnComponentBeginOverlap.AddDynamic(this, &AHAFProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AHAFProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void AHAFProjectile::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // Play local impact AV once (okay on both sides if purely cosmetic; otherwise replicate via cues)
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }
    if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
    }
    if (LoopingSoundComponent)
    {
        LoopingSoundComponent->Stop();
    }

    // Prevent re-entry during the same frame or multiple overlaps
    if (bHasAppliedEffect) { return; }
    bHasAppliedEffect = true;

    // Stop more overlaps immediately
    if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetRootComponent()))
    {
        RootPrim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // Server-only effect application
    if (HasAuthority())
    {
        // Basic validity guards
        if (IsValid(OtherActor) && OtherActor != GetOwner())
        {
            // Resolve target ASC safely (works for actors/components implementing IAbilitySystemInterface)
            UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

            // Resolve source ASC from the instigator (who fired this projectile)
            AActor* InstigatorActor = GetInstigator();
            UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);

            if (IsValid(TargetASC) && IsValid(SourceASC) && IsValid(DamageEffectClass))
            {
                // Build spec *now*, not earlier
                FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
                Ctx.AddSourceObject(this);

                const int32 AbilityLevel = 1; // or your computed level
                FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, Ctx);

                if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            	UE_LOG(LogTemp, Warning, TEXT("Damage On: %s"), *TargetASC->GetName());

            }
        }

        // Give the particle/audio a tick to breathe; or just Destroy() if you prefer
        SetLifeSpan(0.05f);
    }
    else
    {
        // Client cosmetic flag if you need it
        bHit = true;
    }
}


