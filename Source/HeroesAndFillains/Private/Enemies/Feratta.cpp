// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Feratta.h"

#include "Characters/FillainCharacter.h"


AFeratta::AFeratta()
{
    PrimaryActorTick.bCanEverTick = true;

    FerattaStaff = CreateDefaultSubobject<USkeletalMeshComponent>("FerattaStaff");
    FerattaStaff->SetupAttachment(GetMesh(), FName("SpellSocket"));
    FerattaStaff->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a diabolical Feratta"));
}

void AFeratta::BeginPlay()
{
    Super::BeginPlay();


    Tags.Add(FName("Feratta"));
    Tags.Add(FName("Enemy"));

    SpawnEnemyWeapon_Implementation();
}

void AFeratta::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // Clear all timers to prevent crashes
    GetWorldTimerManager().ClearTimer(DamageResetTimer);

    UE_LOG(LogTemp, Error, TEXT("[%s] ❌ EndPlay called (Reason=%d)"), *GetNameSafe(this), (int32)EndPlayReason);

}

void AFeratta::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFeratta::SpawnEnemyWeapon_Implementation()
{
    Super::SpawnEnemyWeapon_Implementation();
}

void AFeratta::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AFeratta::ResetCanDamage, 0.3f, false);
}

void AFeratta::ResetCanDamage()
{
    bCanDamage = true;
    DamagedActors.Empty();
}

void AFeratta::Dissolve()
{
    UE_LOG(LogTemp, Log, TEXT("%s dissolving with custom materials."), *GetName());

    auto ApplyDissolve = [&](UMaterialInterface* Source, void(AFeratta::*StartFunc)(UMaterialInstanceDynamic*))
    {
        if (IsValid(Source))
        {
            UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Source, this);
            GetMesh()->SetMaterial(0, DynMat);
            (this->*StartFunc)(DynMat);
        }
    };

    ApplyDissolve(DissolveMaterialInstanceOne, &AFeratta::StartCharacterDissolveTimelineOne);
    ApplyDissolve(DissolveMaterialInstanceTwo, &AFeratta::StartCharacterDissolveTimelineTwo);
    ApplyDissolve(DissolveMaterialInstanceThree, &AFeratta::StartCharacterDissolveTimelineThree);
    ApplyDissolve(DissolveMaterialInstanceFour, &AFeratta::StartCharacterDissolveTimelineFour);
    ApplyDissolve(DissolveMaterialInstanceFive, &AFeratta::StartCharacterDissolveTimelineFive);
    ApplyDissolve(DissolveMaterialInstanceSix, &AFeratta::StartCharacterDissolveTimelineSix);
}

int32 AFeratta::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}
