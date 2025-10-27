#include "Enemies/StormAssassin.h"

#include "Enemies/Gnarled.h"
#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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
    EnemyDisplayName = FText::FromString(TEXT("a Gnarled"));
}

void AGnarled::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightFistCollision);
    RegisterAttackCollision(LeftFistCollision);

    Tags.Add(FName("Gnarled"));
}

void AGnarled::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGnarled::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if (!bCanDamage || !OtherActor) return;

    Super::OnAttackCollisionOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
                                    SweepResult);

    // Make sure we only hit the player
    AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
    if (!Player || DamagedActors.Contains(Player)) return;

    DamagedActors.Add(Player);

    // Apply damage
    const float DamageAmount = BaseDamage > 0.f ? BaseDamage : 15.f;
    UGameplayStatics::ApplyDamage(Player, DamageAmount, GetController(), this, nullptr);

    // Debug visuals
    FVector HitLocation = OtherActor->GetActorLocation();

    if (!SweepResult.ImpactPoint.IsNearlyZero())
    {
        HitLocation = SweepResult.ImpactPoint;
    }

    DrawDebugSphere(GetWorld(), HitLocation, 20.f, 12, FColor::Red, false, 0.3f, 0, 2);
    UE_LOG(LogTemp, Warning, TEXT("💥 %s hit %s for %.1f damage!"), *GetName(), *GetNameSafe(Player), DamageAmount);

    // Optional: temporary blood Niagara
    // UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodFX, HitLocation);

    // Reset damage after a short delay
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarled::ResetCanDamage, 0.3f, false);
}

void AGnarled::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 AGnarled::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AGnarled::EnableLeftSideMeleeAttack()
{
    if (LeftFistCollision)
    {
        LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Fist Enabled"));
        DrawDebugBox(GetWorld(), LeftFistCollision->GetComponentLocation(),
                     LeftFistCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
}

void AGnarled::DisableLeftSideMeleeAttack()
{
    if (LeftFistCollision)
    {
        LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Fist Disabled"));
    }
}

void AGnarled::EnableRightSideMeleeAttack()
{
    if (RightFistCollision)
    {
        RightFistCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Fist Enabled"));
        DrawDebugBox(GetWorld(), RightFistCollision->GetComponentLocation(),
                     RightFistCollision->GetScaledBoxExtent(),
                     FColor::Cyan, false, 0.25f, 0, 2);
    }
}

void AGnarled::DisableRightSideMeleeAttack()
{
    if (RightFistCollision)
    {
        RightFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Fist Disabled"));
    }
}