#include "Enemies/Gnarledling.h"
#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

AGnarledling::AGnarledling()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Right Fist ---
    RightFistlingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistlingCollision"));
    RightFistlingCollision->SetupAttachment(GetMesh(), FName("RightFistlingSocket"));
    RightFistlingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFistlingCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightFistlingCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightFistlingCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightFistlingCollision->SetGenerateOverlapEvents(true);

    // --- Left Fist ---
    LeftFistlingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistlingCollision"));
    LeftFistlingCollision->SetupAttachment(GetMesh(), FName("LeftFistlingSocket"));
    LeftFistlingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFistlingCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftFistlingCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftFistlingCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftFistlingCollision->SetGenerateOverlapEvents(true);

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a baby Gnarledling"));
}

void AGnarledling::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightFistlingCollision);
    RegisterAttackCollision(LeftFistlingCollision);

    Tags.Add(FName("Gnarledling"));
}

void AGnarledling::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGnarledling::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.3f, false);
}

void AGnarledling::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 AGnarledling::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AGnarledling::EnableLeftSideMeleeAttack()
{
    if (LeftFistlingCollision)
    {
        LeftFistlingCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Fistling Enabled"));
        DrawDebugBox(GetWorld(), LeftFistlingCollision->GetComponentLocation(),
                     LeftFistlingCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
}

void AGnarledling::DisableLeftSideMeleeAttack()
{
    if (LeftFistlingCollision)
    {
        LeftFistlingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Fistling Disabled"));
    }
}

void AGnarledling::EnableRightSideMeleeAttack()
{
    if (RightFistlingCollision)
    {
        RightFistlingCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Fistling Enabled"));
        DrawDebugBox(GetWorld(), RightFistlingCollision->GetComponentLocation(),
                     RightFistlingCollision->GetScaledBoxExtent(),
                     FColor::Cyan, false, 0.25f, 0, 2);
    }
}

void AGnarledling::DisableRightSideMeleeAttack()
{
    if (RightFistlingCollision)
    {
        RightFistlingCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Fistling Disabled"));
    }
}