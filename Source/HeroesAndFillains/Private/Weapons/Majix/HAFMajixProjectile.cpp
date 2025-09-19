// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/HAFMajixProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
	NewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	NewSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	NewSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	
	// Projectile movement pushes the sphere (root)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	if (WeaponBox)
	{
		WeaponBox = CreateDefaultSubobject<UBoxComponent>("WeaponBox");
		WeaponBox->SetupAttachment(GetRootComponent());
		WeaponBox->SetCollisionObjectType(ECC_Projectile);
		WeaponBox->SetGenerateOverlapEvents(true);
		WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		WeaponBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		WeaponBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		WeaponBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		WeaponBox->SetCollisionResponseToChannel(ECC_Enemy, ECR_Block);
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

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}


void AHAFMajixProjectile::OnNewSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
/*    // Play local impact AV once (okay on both sides if purely cosmetic; otherwise replicate via cues)
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
    }*/
}


