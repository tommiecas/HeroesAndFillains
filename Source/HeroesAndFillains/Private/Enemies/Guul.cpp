#include "Enemies/Guul.h"

#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/HitInterface.h"
#include "Perception/AISenseConfig_Hearing.h"

AGuul::AGuul()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Right Fist ---
    RightFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollision"));
    RightFistCollision->SetupAttachment(GetMesh(), FName("RightFistSocket"));
    RightFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightFistCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightFistCollision->SetGenerateOverlapEvents(true);

    // --- Left Fist ---
    LeftFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistCollision"));
    LeftFistCollision->SetupAttachment(GetMesh(), FName("LeftFistSocket"));
    LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftFistCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftFistCollision->SetGenerateOverlapEvents(true);

    // --- Perception ---

    SightConfig->RemoveFromRoot();
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    if (AIPerceptionComponent && HearingConfig)
    {
        HearingConfig->HearingRange =3000.f;
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;

        AIPerceptionComponent->ConfigureSense(*HearingConfig);
        AIPerceptionComponent->SetDominantSense(HearingConfig->GetSenseImplementation());
    }
    
    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a lumbering, horrifying Guul!"));
}

void AGuul::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightFistCollision);
    RegisterAttackCollision(LeftFistCollision);

    Tags.Add(FName("Guul"));
    Tags.Add(FName("Enemy"));
}

void AGuul::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGuul::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
}

void AGuul::Dissolve()
{
    Super::Dissolve();
    UE_LOG(LogTemp, Log, TEXT("%s dissolving with custom materials."), *GetName());

    auto ApplyDissolve = [&](UMaterialInterface* Source, void(AGuul::*StartFunc)(UMaterialInstanceDynamic*))
    {
        if (IsValid(Source))
        {
            UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Source, this);
            GetMesh()->SetMaterial(0, DynMat);
            (this->*StartFunc)(DynMat);
        }
    };
    
}

int32 AGuul::PlayDeathMontage()
{
    const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }
    return Selection;
}

void AGuul::EnableLeftFistMeleeAttack()
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

void AGuul::DisableLeftFistMeleeAttack()
{
    if (LeftFistCollision)
    {
        LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Fist Disabled"));
    }
}

void AGuul::EnableRightFistMeleeAttack()
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

void AGuul::DisableRightFistMeleeAttack()
{
    if (RightFistCollision)
    {
        RightFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Fist Disabled"));
    }
}