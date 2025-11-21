// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Hellspawn.h"

#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"


AHellspawn::AHellspawn()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // --- Teeth ---
    TeethCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TeethCollision"));
    TeethCollision->SetupAttachment(GetMesh(), FName("TeethSocket"));
    TeethCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TeethCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
    TeethCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TeethCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    TeethCollision->SetGenerateOverlapEvents(true);
    
    // Optional: initial display name
    EnemyDisplayName = FText::FromString(TEXT("a demonic Hellspawn"));

    if (HellspawnClass == EHellspawnClass::Bloodspawn)
    {
        IsHellspawnBloodspawn = true;
        IsHellspawnNetherspawn = false;
    }
    if (HellspawnClass == EHellspawnClass::Netherspawn)
    {
        IsHellspawnNetherspawn = true;
        IsHellspawnBloodspawn = false;
    }
}

void AHellspawn::BeginPlay()
{
    Super::BeginPlay();

    RegisterAttackCollision(TeethCollision);

    Tags.Add(FName("Hellspawn"));
    Tags.Add(FName("Enemy"));

    if (HellspawnClass == EHellspawnClass::Bloodspawn) Tags.Add(FName("Bloodspawn"));
    if (HellspawnClass == EHellspawnClass::Netherspawn) Tags.Add(FName("Netherspawn")); 
}

void AHellspawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // Clear all timers to prevent crashes
    GetWorldTimerManager().ClearTimer(TeethDamageResetTimer);

    UE_LOG(LogTemp, Error, TEXT("[%s] ❌ EndPlay called (Reason=%d)"), *GetNameSafe(this), (int32)EndPlayReason);

}

void AHellspawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHellspawn::StartZombieFeast()
{
    if (!CombatTarget) return;

    USkeletalMeshComponent* HellspawnMesh = GetMesh();
    if (AFillainCharacter* Target = Cast<AFillainCharacter>(CombatTarget))
    {
        USkeletalMeshComponent* PlayerMesh = Target->GetMesh();

        if (!HellspawnMesh || !PlayerMesh) return;


    
        if (HellspawnClass == EHellspawnClass::Bloodspawn)
        {
            // 1. Store starting transform
            BloodspawnBiteStartTransform = HellspawnMesh->GetComponentTransform();

            // 2. Compute target socket world transforms
            FTransform FootWorld = PlayerMesh->GetSocketTransform("FootSocket", RTS_World);
            FTransform TeethLocal = HellspawnMesh->GetSocketTransform("TeethSocket", RTS_Component);

            // 3. compute target transform for enemy root so TeethSocket == NeckSocket
            BloodspawnBiteTargetTransform = TeethLocal.Inverse() * FootWorld;

            BloodspawnBiteLerpElapsed = 0.f;
            bIsLerpingToFoot = true;

            GetWorldTimerManager().SetTimer(
                BloodspawnBiteLerpTimer,
                this,
                &AHellspawn::BloodspawnTickBiteLerp,
                0.016f,
                true
            );
            

            UE_LOG(LogTemp, Warning, TEXT("🦇 Enemy attached to %s's foot"), *Target->GetName());
            UE_LOG(LogTemp, Warning, TEXT("🩸 Begin smooth ZombieFeast lerp"));
        }
        else if (HellspawnClass == EHellspawnClass::Netherspawn)
        {
            // 1. Store starting transform
            NetherspawnBiteStartTransform = HellspawnMesh->GetComponentTransform();

            // 2. Compute target socket world transforms
            FTransform NeckWorld = PlayerMesh->GetSocketTransform("NeckSocket", RTS_World);
            FTransform TeethLocal = HellspawnMesh->GetSocketTransform("TeethSocket", RTS_Component);

            // 3. compute target transform for enemy root so TeethSocket == NeckSocket
            NetherspawnBiteTargetTransform = TeethLocal.Inverse() * NeckWorld;

            NetherspawnBiteLerpElapsed = 0.f;
            bIsLerpingToNeck = true;

            GetWorldTimerManager().SetTimer(
                NetherspawnBiteLerpTimer,
                this,
                &AHellspawn::NetherspawnTickBiteLerp,
                0.016f,
                true
            );

            UE_LOG(LogTemp, Warning, TEXT("🦇 Enemy attached to %s's neck"), *Target->GetName());
            UE_LOG(LogTemp, Warning, TEXT("🩸 Begin smooth ZombieFeast lerp"));
        }
    }
}

void AHellspawn::BloodspawnTickBiteLerp()
{
    if (!bIsLerpingToFoot) return;

    BloodspawnBiteLerpElapsed += 0.016f;

    float Alpha = BloodspawnBiteLerpElapsed / BloodspawnBiteLerpDuration;
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // Smoothstep curve
    float SmoothAlpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 4.0f);

    FTransform NewTransform;
    NewTransform.Blend(BloodspawnBiteStartTransform, BloodspawnBiteTargetTransform, SmoothAlpha);

    GetMesh()->SetWorldTransform(NewTransform);

    // Once done — finalize attachment
    if (Alpha >= 1.f)
    {
        bIsLerpingToFoot = false;

        // Stop timer
        GetWorldTimerManager().ClearTimer(BloodspawnBiteLerpTimer);

        // FINAL LOCK: attach enemy mesh to neck socket, preserving final transform
        if (AFillainCharacter* Target = Cast<AFillainCharacter>(CombatTarget))
        {
            GetMesh()->AttachToComponent(
                Target->GetMesh(),
                FAttachmentTransformRules::KeepWorldTransform,
                "FootSocket"
            );

            UE_LOG(LogTemp, Warning, TEXT("🦇 Lerp complete, enemy locked to foot"));
        }
    }
}

void AHellspawn::NetherspawnTickBiteLerp()
{
    if (!bIsLerpingToNeck) return;

    NetherspawnBiteLerpElapsed += 0.016f;

    float Alpha = NetherspawnBiteLerpElapsed / NetherspawnBiteLerpDuration;
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // Smoothstep curve
    float SmoothAlpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 4.0f);

    FTransform NewTransform;
    NewTransform.Blend(NetherspawnBiteStartTransform, NetherspawnBiteTargetTransform, SmoothAlpha);

    GetMesh()->SetWorldTransform(NewTransform);

    // Once done — finalize attachment
    if (Alpha >= 1.f)
    {
        bIsLerpingToNeck = false;

        // Stop timer
        GetWorldTimerManager().ClearTimer(NetherspawnBiteLerpTimer);

        // FINAL LOCK: attach enemy mesh to neck socket, preserving final transform
        if (AFillainCharacter* Target = Cast<AFillainCharacter>(CombatTarget))
        {
            GetMesh()->AttachToComponent(
                Target->GetMesh(),
                FAttachmentTransformRules::KeepWorldTransform,
                "NeckSocket"
            );

            UE_LOG(LogTemp, Warning, TEXT("🦇 Lerp complete, enemy locked to neck"));
        }
    }
}

void AHellspawn::StopZombieFeast()
{
    if (!CombatTarget) return;

    // Stop any pending lerp
    if (AFillainCharacter* Target = Cast<AFillainCharacter>(CombatTarget))
    {
        if (HellspawnClass == EHellspawnClass::Netherspawn)
        {
            GetWorldTimerManager().ClearTimer(NetherspawnBiteLerpTimer);
            bIsLerpingToNeck = false;

            // Unlock from neck
            DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            
        }
        else if (HellspawnClass == EHellspawnClass::Bloodspawn)
        {
            GetWorldTimerManager().ClearTimer(BloodspawnBiteLerpTimer);
            bIsLerpingToFoot = false;

            // Unlock from neck
            DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            
        }
        
        // GAS damage
        if (UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent())
        {
            if (ZombieFeastDamageEffect)
            {
                FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
                Context.AddSourceObject(this);

                FGameplayEffectSpecHandle SpecHandle =
                ASC->MakeOutgoingSpec(ZombieFeastDamageEffect, 1.f, Context);

                if (SpecHandle.IsValid())
                {
                    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("💥 Zombie Feast damage applied to %s!"),
                       *Target->GetName());

            UE_LOG(LogTemp, Warning, TEXT("💥 Bite ended + damage applied"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Cannot apply blood suck damage — ASC or GE missing."));
        }
        UE_LOG(LogTemp, Warning, TEXT("💥 Zombie Feast ended — damage applied & detached."));
    }
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
    GetWorldTimerManager().SetTimer(TeethDamageResetTimer, this, &AHellspawn::ResetCanDamage, 0.3f, false);
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

void AHellspawn::EnableTeethMeleeAttack()
{
    if (TeethCollision)
    {
        TeethCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        UE_LOG(LogTemp, Warning, TEXT("🟢 Teeth Enabled"));
        DrawDebugBox(GetWorld(), TeethCollision->GetComponentLocation(),
                     TeethCollision->GetScaledBoxExtent(),
                     FColor::Green, false, 0.25f, 0, 2);
    }
}

void AHellspawn::DisableTeethMeleeAttack()
{
    if (TeethCollision)
    {
        TeethCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("🔴 Teeth Disabled"));
    }
}
