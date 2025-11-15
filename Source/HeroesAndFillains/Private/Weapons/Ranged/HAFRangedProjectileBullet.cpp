// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/HAFRangedProjectileBullet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Kismet/GameplayStatics.h"


AHAFRangedProjectileBullet::AHAFRangedProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	
	NewSphere = CreateDefaultSubobject<USphereComponent>(TEXT("NewSphere"));
	NewSphere->SetupAttachment(RootComponent);
	NewSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NewSphere->SetCollisionObjectType(ECC_Projectile);
	NewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	NewSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	NewSphere->IgnoreActorWhenMoving(this, true);
	
	// Projectile movement pushes the sphere (root)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	/*ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;*/

	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s"), *GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile rotation: %s"), *GetActorRotation().ToString());
}

void AHAFRangedProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	if (ensureMsgf(CollisionBox != nullptr, TEXT("CollisionBox was not created")))
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AHAFRangedProjectileBullet::OnCollisionBoxOverlap);
	}

	if (AActor* MyOwner = GetOwner())
	{
		CollisionBox->IgnoreActorWhenMoving(MyOwner, true);
	}
	
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s"), *GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile rotation: %s"), *GetActorRotation().ToString());
	if (LoopingSound && LoopingSoundComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Looping Sound of %s"), *LoopingSound->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Looping Sound Component of %s"), *LoopingSoundComponent->GetName());
	}
	if (ImpactNiagaraSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has Impact Effect of %s"), *ImpactNiagaraSystem->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("Projectile spawned at %s with velocity %s"), *GetActorLocation().ToString(), *ProjectileMovement->Velocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Projectile BeginPlay Location: %s, Velocity: %s"), *GetActorLocation().ToString(), *ProjectileMovement->Velocity.ToString());
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + ProjectileMovement->Velocity * 0.1f, FColor::Green, false, 3.0f, 0, 5.0f);
  

}

void AHAFRangedProjectileBullet::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
		if (ImpactNiagaraSystem) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactNiagaraSystem, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}


void AHAFRangedProjectileBullet::OnCollisionBoxOverlap(
    UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
	if (!BulletDamageEffectSpecHandle.Data.IsValid() || BulletDamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor) return;

	if (!UHAFAbilitySystemBlueprintLibrary::IsNotFriend(BulletDamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser(), OtherActor)) return;
	
    UE_LOG(LogTemp, Warning, TEXT("🔥 Projectile overlap! Hit: %s"), *GetNameSafe(OtherActor));

    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactNiagaraSystem, GetActorLocation());
        if (LoopingSoundComponent) LoopingSoundComponent->Stop();

        if (HasAuthority())
        {
            if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("✅ Target %s has ASC"), *GetNameSafe(OtherActor));
                
                if (BulletDamageEffectSpecHandle.IsValid())
                {
                    UE_LOG(LogTemp, Warning, TEXT("✅ DamageEffectSpecHandle is VALID"));
                    
                    // ✅ Log all SetByCaller values in the spec
                    const FGameplayEffectSpec* Spec = BulletDamageEffectSpecHandle.Data.Get();
                    if (Spec)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("📋 Spec has %d SetByCaller magnitudes"), 
                            Spec->SetByCallerTagMagnitudes.Num());
                        
                        for (const auto& Pair : Spec->SetByCallerTagMagnitudes)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("  - Tag: %s | Value: %.1f"), 
                                *Pair.Key.ToString(), Pair.Value);
                        }
                    }
                    
                    // ✅ Apply damage effect to the target
                    FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
                    
                    if (ActiveHandle.IsValid())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("💥 Successfully applied damage effect!"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("❌ Failed to apply damage effect!"));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("❌ DamageEffectSpecHandle is INVALID!"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("⚠️ Hit actor %s has no AbilitySystemComponent"), *GetNameSafe(OtherActor));
            }
            Destroy();
        }
        else
        {
            bHit = true;
        }
    }
}



void AHAFRangedProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

