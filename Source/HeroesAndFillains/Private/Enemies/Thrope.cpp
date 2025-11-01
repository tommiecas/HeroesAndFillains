
#include "Enemies/Thrope.h"
#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AThrope::AThrope()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Right Fist ---
    RightHandClawsCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandClawsCollision"));
    RightHandClawsCollision->SetupAttachment(GetMesh(), FName("RightHandClawsSocket"));
    RightHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightHandClawsCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightHandClawsCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightHandClawsCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightHandClawsCollision->SetGenerateOverlapEvents(true);

    // --- Left Fist ---
    LeftHandClawsCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandClawsCollision"));
    LeftHandClawsCollision->SetupAttachment(GetMesh(), FName("LeftHandClawsSocket"));
    LeftHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftHandClawsCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftHandClawsCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftHandClawsCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftHandClawsCollision->SetGenerateOverlapEvents(true);


    // --- Right Foot ---
    RightFootClawsCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFootClawsClawsCollision"));
    RightFootClawsCollision->SetupAttachment(GetMesh(), FName("RightFootClawsSocket"));
    RightFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFootClawsCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightFootClawsCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightFootClawsCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightFootClawsCollision->SetGenerateOverlapEvents(true);

    // --- Left Foot ---
    LeftFootClawsCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFootClawsCollision"));
    LeftFootClawsCollision->SetupAttachment(GetMesh(), FName("LeftFootClawsSocket"));
    LeftFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFootClawsCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftFootClawsCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftFootClawsCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftFootClawsCollision->SetGenerateOverlapEvents(true);

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a Gnarled"));
}

void AThrope::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightHandClawsCollision);
    RegisterAttackCollision(LeftHandClawsCollision);
    RegisterAttackCollision(RightFootClawsCollision);
    RegisterAttackCollision(LeftFootClawsCollision);

    Tags.Add(FName("Thrope"));
}

void AThrope::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AThrope::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
    const float FootDamageAmount = FootClawsDamage > 0.f ? FootClawsDamage : 25.f;
    UGameplayStatics::ApplyDamage(Player, FootDamageAmount, GetController(), this, nullptr);
    const float HandDamageAmount = HandClawsDamage > 0.f ? HandClawsDamage : 35.f;
    UGameplayStatics::ApplyDamage(Player, HandDamageAmount, GetController(), this, nullptr);

    // Debug visuals
    FVector HitLocation = OtherActor->GetActorLocation();

    if (!SweepResult.ImpactPoint.IsNearlyZero())
    {
        HitLocation = SweepResult.ImpactPoint;
    }

    DrawDebugSphere(GetWorld(), HitLocation, 20.f, 12, FColor::Red, false, 0.3f, 0, 2);
    UE_LOG(LogTemp, Warning, TEXT("💥 %s hit %s for %.1f foot damage!"), *GetName(), *GetNameSafe(Player), FootDamageAmount);
    UE_LOG(LogTemp, Warning, TEXT("💥 %s hit %s for %.1f hand damage!"), *GetName(), *GetNameSafe(Player), HandDamageAmount);

    // Optional: temporary blood Niagara
    // UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodFX, HitLocation);

    // Reset damage after a short delay
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(HandClawsResetTimer, this, &AThrope::ResetCanDamage, 0.3f, false);
    GetWorldTimerManager().SetTimer(FootClawsResetTimer, this, &AThrope::ResetCanDamage, 0.3f, false);
}

void AThrope::Dissolve()
{
    // --- optional visual dissolve code here ---
    // e.g. spawn dynamic material instances and run dissolve timelines
}

int32 AThrope::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AThrope::EnableLeftSideMeleeAttack()
{
    if (LeftHandClawsCollision)
    {
        LeftHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Hand Claws Enabled"));
        DrawDebugBox(GetWorld(), LeftHandClawsCollision->GetComponentLocation(),
                     LeftHandClawsCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
    if (LeftFootClawsCollision)
    {
        LeftFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Foot Enabled"));
        DrawDebugBox(GetWorld(), LeftFootClawsCollision->GetComponentLocation(),
                     LeftFootClawsCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
}

void AThrope::DisableLeftSideMeleeAttack()
{
    if (LeftHandClawsCollision)
    {
        LeftHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Hand Claws Disabled"));
    }
    if (LeftFootClawsCollision)
    {
        LeftFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Foot Claws Disabled"));
    }
}

void AThrope::EnableRightSideMeleeAttack()
{
    if (RightHandClawsCollision)
    {
        RightHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Hand Claws Enabled"));
        DrawDebugBox(GetWorld(), RightHandClawsCollision->GetComponentLocation(),
                     RightHandClawsCollision->GetScaledBoxExtent(),
                     FColor::Cyan, false, 0.25f, 0, 2);
    }
    if (RightFootClawsCollision)
    {
        RightFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Foot Claws Enabled"));
        DrawDebugBox(GetWorld(), RightFootClawsCollision->GetComponentLocation(),
                     RightFootClawsCollision->GetScaledBoxExtent(),
                     FColor::Cyan, false, 0.25f, 0, 2);
    }
}

void AThrope::DisableRightSideMeleeAttack()
{
    if (RightHandClawsCollision)
    {
        RightHandClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Hand Claws Disabled"));
    }
    if (RightFootClawsCollision)
    {
        RightFootClawsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Foot Claws Disabled"));
    }
}