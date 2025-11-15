// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/SpectralBase.h"

#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"


ASpectralBase::ASpectralBase()
{

	PrimaryActorTick.bCanEverTick = true;

    SpectralAssaultRifle = CreateDefaultSubobject<USkeletalMeshComponent>("AssaultRifle");
    SpectralAssaultRifle->SetupAttachment(GetMesh(), FName("RangedSocket"));
    SpectralAssaultRifle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Optional: initial display name
	EnemyDisplayName = FText::FromString(TEXT("a relentless Spectral"));
}

// Called when the game starts or when spawned
void ASpectralBase::BeginPlay()
{
	Super::BeginPlay();

	// ✅ Register these colliders with the inherited attack system

	Tags.Add(FName("Spectral"));
	
}

// Called every frame
void ASpectralBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpectralBase::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if (!bCanDamage || !OtherActor) return;

    Super::OnAttackCollisionOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
                                    SweepResult);

    // Make sure we only hit the player
    AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
    if (!Player || DamagedActors.Contains(Player)) return;

    DamagedActors.Add(Player);

    // Damage is now handled through GAS via GetHit_Implementation
    FVector HitLocation = OtherActor->GetActorLocation();
    if (!SweepResult.ImpactPoint.IsNearlyZero())
    {
        HitLocation = SweepResult.ImpactPoint;
    }

    if (IHitInterface* HitInterface = Cast<IHitInterface>(Player))
    {
        HitInterface->Execute_GetHit(Player, HitLocation, this);
        UE_LOG(LogTemp, Warning, TEXT("💥 %s hit %s via GAS!"), *GetName(), *GetNameSafe(Player));
    }

    // Debug visuals
    DrawDebugSphere(GetWorld(), HitLocation, 20.f, 12, FColor::Red, false, 0.3f, 0, 2);

    // Reset damage after a short delay
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &ASpectralBase::ResetCanDamage, 0.3f, false);
}

void ASpectralBase::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 ASpectralBase::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}