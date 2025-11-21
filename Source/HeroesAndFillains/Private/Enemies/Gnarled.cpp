#include "Enemies/Gnarled.h"
#include "Components/BoxComponent.h"
#include "Characters/FillainCharacter.h"
#include "GameFramework/DamageType.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/HitInterface.h"

AGnarled::AGnarled()
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

    // --- Right Foot ---
    RightFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFootCollision"));
    RightFootCollision->SetupAttachment(GetMesh(), FName("RightFootSocket"));
    RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    RightFootCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    RightFootCollision->SetGenerateOverlapEvents(true);

    // --- Left Foot ---
    LeftFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFootCollision"));
    LeftFootCollision->SetupAttachment(GetMesh(), FName("LeftFootSocket"));
    LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    LeftFootCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    LeftFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    LeftFootCollision->SetGenerateOverlapEvents(true);

    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a ginormous Gnarled! Don't piss off Papa!"));
}

void AGnarled::BeginPlay()
{
    Super::BeginPlay();

    // ✅ Register these colliders with the inherited attack system
    RegisterAttackCollision(RightFistCollision);
    RegisterAttackCollision(LeftFistCollision);
    RegisterAttackCollision(RightFootCollision);
    RegisterAttackCollision(LeftFootCollision);

    Tags.Add(FName("Gnarled"));
    Tags.Add(FName("Enemy"));
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

void AGnarled::Dissolve()
{
    Super::Dissolve();
    UE_LOG(LogTemp, Log, TEXT("%s dissolving with custom materials."), *GetName());

    auto ApplyDissolve = [&](UMaterialInterface* Source, void(AGnarled::*StartFunc)(UMaterialInstanceDynamic*))
    {
        if (IsValid(Source))
        {
            UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Source, this);
            GetMesh()->SetMaterial(0, DynMat);
            (this->*StartFunc)(DynMat);
        }
    };

    ApplyDissolve(DissolveMaterialInstanceOne, &AGnarled::StartCharacterDissolveTimelineOne);
    ApplyDissolve(DissolveMaterialInstanceTwo, &AGnarled::StartCharacterDissolveTimelineTwo);
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
    
    if (LeftFootCollision)
    {
        LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Left Foot Enabled"));
        DrawDebugBox(GetWorld(), LeftFootCollision->GetComponentLocation(),
            LeftFootCollision->GetScaledBoxExtent(),
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

    if (LeftFootCollision)
    {
        LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Left Foot Disabled"));
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

    if (RightFootCollision)
    {
        RightFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Right Foot Enabled"));
        DrawDebugBox(GetWorld(), RightFootCollision->GetComponentLocation(),
                     RightFootCollision->GetScaledBoxExtent(),
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

    if (RightFootCollision)
    {
        RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Right Foot Disabled"));
    }
}