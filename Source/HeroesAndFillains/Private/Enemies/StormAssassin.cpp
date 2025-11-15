#include "Enemies/StormAssassin.h"

#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

AStormAssassin::AStormAssassin()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Right Fist ---
    RightFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollision"));
    RightFootCollision->SetupAttachment(GetMesh(), FName("RightFistSocket"));
    RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightFootCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightFootCollision->SetGenerateOverlapEvents(true);

    // --- Left Fist ---
    LeftFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistCollision"));
    LeftFootCollision->SetupAttachment(GetMesh(), FName("LeftFistSocket"));
    LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftFootCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftFootCollision->SetGenerateOverlapEvents(true);

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a fearsome Storm Assassin"));
}

void AStormAssassin::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightFootCollision);
    RegisterAttackCollision(LeftFootCollision);

    Tags.Add(FName("StormAssassin"));
}

void AStormAssassin::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AStormAssassin::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AStormAssassin::ResetCanDamage, 0.3f, false);
}

void AStormAssassin::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 AStormAssassin::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AStormAssassin::EnableLeftSideMeleeAttack()
{
    if (LeftFootCollision)
    {
        LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Fist Enabled"));
        DrawDebugBox(GetWorld(), LeftFootCollision->GetComponentLocation(),
                     LeftFootCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
}

void AStormAssassin::DisableLeftSideMeleeAttack()
{
    if (LeftFootCollision)
    {
        LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Foot Disabled"));
    }
}

void AStormAssassin::EnableRightSideMeleeAttack()
{
    if (RightFootCollision)
    {
        RightFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Fist Enabled"));
        DrawDebugBox(GetWorld(), RightFootCollision->GetComponentLocation(),
                     RightFootCollision->GetScaledBoxExtent(),
                     FColor::Cyan, false, 0.25f, 0, 2);
    }
}

void AStormAssassin::DisableRightSideMeleeAttack()
{
    if (RightFootCollision)
    {
        RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Foot Disabled"));
    }
}