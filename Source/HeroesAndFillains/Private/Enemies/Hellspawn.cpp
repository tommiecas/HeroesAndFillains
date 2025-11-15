// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Hellspawn.h"

#include "Characters/FillainCharacter.h"


AHellspawn::AHellspawn()
{
    PrimaryActorTick.bCanEverTick = true;
    

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a demonic Hellspawn"));
}

void AHellspawn::BeginPlay()
{
    Super::BeginPlay();


    Tags.Add(FName("Hellspawn"));
    Tags.Add(FName("Enemy"));
}

void AHellspawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // Clear all timers to prevent crashes
    GetWorldTimerManager().ClearTimer(DamageResetTimer);

    UE_LOG(LogTemp, Error, TEXT("[%s] ❌ EndPlay called (Reason=%d)"), *GetNameSafe(this), (int32)EndPlayReason);

}

void AHellspawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHellspawn::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if (!bCanDamage || !OtherActor) return;

    Super::OnAttackCollisionOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
                                    SweepResult);

    // Make sure we only hit the player
    AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
    if (!Player || DamagedActors.Contains(Player)) return;

    DamagedActors.Add(Player);

    // ✅ Add damage reset logic
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AHellspawn::ResetCanDamage, 0.3f, false);
}

void AHellspawn::ResetCanDamage()
{
    bCanDamage = true;
    DamagedActors.Empty();
}

void AHellspawn::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 AHellspawn::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}
