#include "Enemies/EnemyBase.h"
#include "AIController.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "UI/Widgets/EnemyStatsWidget.h"
#include "UI/Widgets/EnemyAttributeMenuWidget.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "AbilitySystem/AttributeIhfo.h"
#include "Engine/Engine.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "UI/FillainHUD.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Items/Soul.h"
#include "GameplayTags.h"
#include "HAFGameplayTags.h"
#include "MotionWarpingComponent.h"
#include "AI/HAFAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "UI/WidgetControllers/EnemyWidgetControllerBase.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));

    
    // --- Ability System ---
    EnemyAbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    EnemyAttributeSet = CreateDefaultSubobject<UHAFAttributeSet>(TEXT("AttributeSet"));

    // --- Perception ---
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    if (AIPerceptionComponent && SightConfig)
    {
        SightConfig->SightRadius = 1200.f;
        SightConfig->LoseSightRadius = 1400.f;
        SightConfig->PeripheralVisionAngleDegrees = 90.f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }

    // --- Stats Widget ---
    EnemyStatsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyStatsWidgetComponent"));
    EnemyStatsWidgetComponent->SetupAttachment(RootComponent);
    EnemyStatsWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    EnemyStatsWidgetComponent->SetDrawAtDesiredSize(true);
    EnemyStatsWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
    EnemyStatsWidgetComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AEnemyBase::SetWarpTargetsForCombatTarget(AActor* TargetActor)
{
    if (!MotionWarpingComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no MotionWarpingComp!"), *GetName());
        return;
    }

    if (!IsValid(TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Invalid TargetActor for motion warping."), *GetName());
        return;
    }

    const FVector TargetLocation = TargetActor->GetActorLocation();
    const FRotator TargetRotation = TargetActor->GetActorRotation();

    // ✅ These are the correct modern calls
    MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
        FName("TranslationTarget"),
        TargetLocation,
        TargetRotation);

    MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
        FName("RotationTarget"),
        TargetLocation,
        TargetRotation);

    UE_LOG(LogTemp, Log, TEXT("%s set Motion Warping targets for %s (Translation + Rotation)."),
        *GetName(), *GetNameSafe(TargetActor));
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    // --- Initialize AI Perception ---
    if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound() == false)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBase::OnTargetDetected);
    }
    else if (!AIPerceptionComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: AIPerceptionComponent is null in BeginPlay!"), *GetName());
    }

    // --- Defensive sanity checks ---
    if (!EnemyStatsWidgetComponent)
    {
        EnemyStatsWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("EnemyStatsWidgetComponent"));
        EnemyStatsWidgetComponent->SetupAttachment(GetRootComponent());
        EnemyStatsWidgetComponent->RegisterComponent();
    }

    // --- GAS setup ---
    InitializeAbilityActorInfo();
    InitializeDefaultAttributes();
    if (EnemyWidgetControllerClass)
    {
        EnemyWidgetController = NewObject<UEnemyWidgetControllerBase>(this, EnemyWidgetControllerClass);
        if (EnemyAttributeInfoOverride && EnemyWidgetController)
        {
            EnemyWidgetController->SetEnemyAttributeInfo(EnemyAttributeInfoOverride);
            UE_LOG(LogTemp, Log, TEXT("[%s] Overrode AttributeInfo with %s"),
                *GetName(), *GetNameSafe(EnemyAttributeInfoOverride));
        }
    }

    if (EnemyWidgetController)
    {
        EnemyWidgetController->Initialize(this);

        // 🔹 Bind and prepare widget
        InitializeEnemyWidgets();

        // ✅ Force an immediate sync
        EnemyWidgetController->BroadcastInitialEnemyValues();

        // 🔹 And do the delayed broadcast as before
        TWeakObjectPtr<UEnemyWidgetControllerBase> WeakController = EnemyWidgetController;
        GetWorldTimerManager().SetTimerForNextTick([this, WeakController]()
        {
            if (WeakController.IsValid())
            {
                WeakController->BroadcastInitialEnemyValues();
            }
        });
    }

    // 🔹 Delayed broadcast (to catch post-init GAS updates)
    TWeakObjectPtr<UEnemyWidgetControllerBase> WeakController = EnemyWidgetController;
    GetWorldTimerManager().SetTimerForNextTick([this, WeakController]()
    {
        if (WeakController.IsValid())
        {
            WeakController->BroadcastInitialEnemyValues();
        }
    });

    // --- Movement defaults ---
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->MaxWalkSpeed = 300.f;
    }

    // --- Deferred initialization ---
    FTimerHandle DeferredHandle;
    GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyBase::InitializeEnemyAttributeMenu);

    if (MotionWarpingComponent)
    {
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            FName("TranslationTarget"),
            GetActorLocation(),
            GetActorRotation()
        );

        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            FName("RotationTarget"),
            GetActorLocation(),
            GetActorRotation()
        );
    }
}

void AEnemyBase::InitializeDefaultAttributes() const
{
    Super::InitializeDefaultAttributes();
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Follow mouse when visible
    if (ActiveAttributeMenuWidget && ActiveAttributeMenuWidget->IsVisible())
    {
        FVector2D MousePos;
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->GetMousePosition(MousePos.X, MousePos.Y);
            ActiveAttributeMenuWidget->SetPositionInViewport(MousePos + FVector2D(20.f, 20.f), true);
        }
    }
    Super::Tick(DeltaTime);

    if (CombatTarget && MotionWarpingComponent)
    {
        const FVector TargetLocation = CombatTarget->GetActorLocation();
        const FRotator TargetRotation = (TargetLocation - GetActorLocation()).Rotation();

        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            FName("TranslationTarget"), TargetLocation, TargetRotation);

        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            FName("RotationTarget"), TargetLocation, TargetRotation);
    }
}

void AEnemyBase::InitializeEnemyWidgets()
{
    if (!EnemyStatsWidgetComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Missing EnemyStatsWidgetComponent — creating one dynamically."), *GetName());
        EnemyStatsWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("EnemyStatsWidgetComponent"));
        EnemyStatsWidgetComponent->SetupAttachment(GetRootComponent());
        EnemyStatsWidgetComponent->RegisterComponent();
    }

    // --- Ensure the widget is properly initialized ---
    EnemyStatsWidgetComponent->InitWidget();

    // --- Get and validate the UserWidget ---
    UUserWidget* RawWidget = EnemyStatsWidgetComponent->GetUserWidgetObject();
    if (!RawWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] EnemyStatsWidgetComponent has no UserWidgetObject!"), *GetName());
        return;
    }

    // --- Try to cast to our type ---
    if (UEnemyStatsWidget* StatsWidget = Cast<UEnemyStatsWidget>(RawWidget))
    {
        EnemyStatsWidget = StatsWidget;

        if (EnemyWidgetController)
        {
            StatsWidget->SetWidgetController(EnemyWidgetController);
            UE_LOG(LogTemp, Log, TEXT("[%s] Successfully initialized EnemyStatsWidget with controller: %s"),
                *GetName(), *GetNameSafe(EnemyWidgetController));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] StatsWidget initialized but EnemyWidgetController is nullptr!"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] EnemyStatsWidgetComponent UserWidget is not of class UEnemyStatsWidget (got: %s)"),
            *GetName(), *GetNameSafe(RawWidget->GetClass()));
    }
}

void AEnemyBase::InitializeEnemyAttributeMenu()
{
    if (!EnemyAttributeMenuWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;
    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(PC, EnemyAttributeMenuWidgetClass);
    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->AddToViewport(10);
        ActiveAttributeMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        ActiveAttributeMenuWidget->SetWidgetController(EnemyWidgetController);
        UE_LOG(LogTemp, Log, TEXT("Created viewport-based ActiveAttributeMenuWidget for %s."), *GetName());
    }
}

void AEnemyBase::OnHoverStart()
{
    if (bIsHovered) return; // 👈 Prevent re-triggering
    bIsHovered = true;

    if (!CachedPC)
        CachedPC = UGameplayStatics::GetPlayerController(this, 0);

    if (!CachedPC || !EnemyAttributeMenuWidgetClass)
        return;

    // ✅ Create the widget once
    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(CachedPC, EnemyAttributeMenuWidgetClass);
    if (!ActiveAttributeMenuWidget) return;

    ActiveAttributeMenuWidget->AddToViewport(10);
    ActiveAttributeMenuWidget->SetWidgetController(EnemyWidgetController);
    ActiveAttributeMenuWidget->FadeIn(0.2f); // smoother than ShowTemporarily

    FVector2D MousePos;
    CachedPC->GetMousePosition(MousePos.X, MousePos.Y);
    ActiveAttributeMenuWidget->SetPositionInViewport(MousePos + FVector2D(20.f, 20.f), true);

    UE_LOG(LogTemp, Log, TEXT("%s: Hover menu opened once"), *GetName());
}

void AEnemyBase::OnHoverEnd()
{
    if (!bIsHovered) return; // 👈 Only close if it was actually open
    bIsHovered = false;

    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->FadeOut(0.2f);

        FTimerHandle CleanupTimer;
        GetWorld()->GetTimerManager().SetTimer(
            CleanupTimer,
            [this]()
            {
                if (ActiveAttributeMenuWidget)
                {
                    ActiveAttributeMenuWidget->RemoveFromParent();
                    ActiveAttributeMenuWidget = nullptr;
                }
            },
            0.25f,
            false
        );

        UE_LOG(LogTemp, Log, TEXT("%s: Hover menu closed"), *GetName());
    }
}


void AEnemyBase::UpdateEnemyAttributeMenu()
{
    if (!EnemyAttributeMenuWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no EnemyAttributeMenuWidget to update!"), *GetName());
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    FVector2D ScreenPosition;
    if (PC->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPosition))
    {
        ActiveAttributeMenuWidget->SetPositionInViewport(ScreenPosition + FVector2D(20.f, 0.f));
    }

    UE_LOG(LogTemp, Log, TEXT("%s EnemyAttributeMenuWidget updated successfully"), *GetName());
}

void AEnemyBase::InitializeAbilityActorInfo()
{
    if (!EnemyAbilitySystemComponent) return;
    EnemyAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AEnemyBase::InitializeEnemy()
{
    // Spawn default weapon, initialize AI state, etc.
    SpawnEnemyWeapon();
    UE_LOG(LogTemp, Log, TEXT("Enemy %s initialized."), *GetName());
}

void AEnemyBase::SpawnEnemyWeapon()
{
    if (!HasAuthority()) return;

    if (!EquippedEnemyWeapon)
    {
        // Replace this with your actual weapon blueprint reference
        UE_LOG(LogTemp, Log, TEXT("Spawning weapon for %s..."), *GetName());
    }
}

void AEnemyBase::CheckPatrolTarget()
{
    // Simplified example
    if (!EnemyController) return;
    UE_LOG(LogTemp, Verbose, TEXT("%s checking patrol target"), *GetName());
}

void AEnemyBase::CheckCombatTarget()
{
    UE_LOG(LogTemp, Verbose, TEXT("%s checking combat target"), *GetName());
}

void AEnemyBase::EnemiesChaseTarget()
{
    UE_LOG(LogTemp, Verbose, TEXT("%s chasing target"), *GetName());
}

bool AEnemyBase::IsOutsideCombatRadius() { return false; }
bool AEnemyBase::IsOutsideAttackRadius() { return false; }
bool AEnemyBase::IsInsideAttackRadius() { return true; }
bool AEnemyBase::IsEnemyChasing() { return false; }
bool AEnemyBase::IsEnemyAttacking() { return false; }
bool AEnemyBase::IsEnemyEngaged() { return false; }
bool AEnemyBase::IsEnemyDead() { return false; }

void AEnemyBase::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemyBase::StartAttackTimer()
{
    const float RandTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyBase::AttackEnd, RandTime);
}

void AEnemyBase::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemyBase::AttackEnd()
{
    EnemyState = EEnemyState::EES_Idle;
    bCanDamage = false;
    DamagedActors.Empty();
    
    if (EnemyController && CombatTarget)
    {
        // Resume chasing or return to patrol
        EnemyController->MoveToActor(CombatTarget, AcceptanceRadius);
    }
    
    UE_LOG(LogTemp, Log, TEXT("%s finished attacking"), *GetName());
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    HandleDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // Set combat target if not already set
    if (!CombatTarget && EventInstigator)
    {
        if (APawn* InstigatorPawn = EventInstigator->GetPawn())
        {
            CombatTarget = InstigatorPawn;
        }
    }
    
    return DamageAmount;
}

void AEnemyBase::HandleDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage from %s"), *GetName(), DamageAmount,
        *GetNameSafe(DamageCauser));

    if (!EnemyAbilitySystemComponent || !EnemyAttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Missing ASC or AttributeSet in HandleDamage!"), *GetName());
        return;
    }

    // Apply damage through GAS
    if (DamageEffectClass)
    {
        FGameplayEffectContextHandle EffectContext = EnemyAbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(DamageCauser);
        EffectContext.AddInstigator(EventInstigator ? EventInstigator->GetPawn() : nullptr, DamageCauser);

        FGameplayEffectSpecHandle SpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(
            DamageEffectClass, 
            1.0f, 
            EffectContext
        );

        if (SpecHandle.IsValid())
        {
            // Set damage magnitude using SetByCaller
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(FName("Data.Damage")), 
                DamageAmount
            );

            EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    // Show hit react
    if (IsValid(DamageCauser))
    {
        GetHit_Implementation(GetActorLocation(), DamageCauser);
    }

    // Check for death
    const float CurrentHealth = EnemyAttributeSet->GetHealth();
    if (CurrentHealth <= 0.0f && !bDead)
    {
        Die();
    }
}

void AEnemyBase::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    UE_LOG(LogTemp, Log, TEXT("%s hit by %s at %s"), *GetName(), *GetNameSafe(Hitter), *ImpactPoint.ToString());

    // Stop movement temporarily
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->StopMovementImmediately();
    }

    // Set combat target
    if (!CombatTarget)
    {
        CombatTarget = Hitter;
    }

    // Play hit reaction animation
    if (HitReactMontage && !bDead)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
        {
            // Calculate hit direction for directional hit reacts
            const FVector Forward = GetActorForwardVector();
            const FVector ImpactDirection = (ImpactPoint - GetActorLocation()).GetSafeNormal();
            const double DotProduct = FVector::DotProduct(Forward, ImpactDirection);
            
            FName SectionName = FName("FromFront");
            if (DotProduct < -0.5)
            {
                SectionName = FName("FromBack");
            }
            
            AnimInstance->Montage_Play(HitReactMontage);
            AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
        }
    }

    // Visual effects at impact point
    if (HitParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
    }

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
    }
}

void AEnemyBase::Dissolve()
{
    UE_LOG(LogTemp, Log, TEXT("%s beginning dissolve effect."), *GetName());
    if (UMaterialInterface* DissolveMaterialBase = GetMesh()->GetMaterial(0))
    {
        // Create dynamic material instance for dissolve effect
        UMaterialInstanceDynamic* DynamicDissolveMat = UMaterialInstanceDynamic::Create(
            DissolveMaterialBase, 
            this
        );
        
        GetMesh()->SetMaterial(0, DynamicDissolveMat);
        // Start dissolve timeline
        if (DissolveTimeline)
        {
            DissolveTimeline->PlayFromStart();
        }
    }
    // Disable collision during dissolve
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Destroy after dissolve duration
    SetLifeSpan(3.0f);
}

void AEnemyBase::Die()
{
    if (bDead) return; // Prevent multiple death calls
    bDead = true;
    UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());
    // Clean up UI
    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->RemoveFromParent();
        ActiveAttributeMenuWidget = nullptr;
    }
    if (EnemyStatsWidgetComponent)
    {
        EnemyStatsWidgetComponent->SetVisibility(false);
    }
    // Stop AI
    if (EnemyController)
    {
        EnemyController->StopMovement();
        if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsBool(FName("IsDead"), true);
        }
    }
    // Disable collision
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    // Stop movement
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->StopMovementImmediately();
        Movement->DisableMovement();
    }
    // Play death animation
    PlayDeathMontage();
    // Multicast to clients
    MulticastHandleDeath_Implementation();
    // Start dissolve effect
    Dissolve();
    // Spawn soul
    SpawnSoul();
    // Destroy after delay (let animations/dissolve finish)
    SetLifeSpan(5.0f);
}

void AEnemyBase::MulticastHandleDeath_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("%s multicast handle death."), *GetName());
    // Visual effects on all clients
    if (DeathParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            DeathParticles,
            GetActorLocation(),
            FRotator::ZeroRotator,
            FVector(1.f)
        );
    }
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            DeathSound,
            GetActorLocation()
        );
    }
    // Weapon handling
    if (EquippedEnemyWeapon)
    {
        EquippedEnemyWeapon->SetActorHiddenInGame(true);
    }
}

int32 AEnemyBase::PlayDeathMontage()
{
    if (!DeathMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no DeathMontage assigned!"), *GetName());
        return -1;
    }
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        return -1;
    }
    // Pick random death animation
    const int32 NumSections = DeathMontage->CompositeSections.Num();
    if (NumSections == 0)
    {
        AnimInstance->Montage_Play(DeathMontage);
        return 0;
    }
    const int32 Selection = FMath::RandRange(0, NumSections - 1);
    const FName SectionName = DeathMontage->GetSectionName(Selection);
    AnimInstance->Montage_Play(DeathMontage);
    AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
    UE_LOG(LogTemp, Log, TEXT("%s playing death montage section %d: %s"), 
        *GetName(), Selection, *SectionName.ToString());
    return Selection;
}


void AEnemyBase::MeleeAttack()
{
    if (!CanAttack()) return;
    SetWarpTargetsForCombatTarget(CombatTarget);
    PlayRandomMeleeAttackMontage();
}

void AEnemyBase::MajixAttack()
{
    if (!CanAttack()) return;
    UE_LOG(LogTemp, Log, TEXT("%s performing majix attack."), *GetName());
        PlayRandomMajixAttackMontage();
}

void AEnemyBase::PlayAttackMontage()
{
    if (!MeleeAttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no AttackMontage assigned!"), *GetName());
        return;
    }
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;
    if (AnimInstance->IsAnyMontagePlaying())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s tried to attack while montage already playing"), *GetName());
        return;
    }
    // Set state
    EnemyState = EEnemyState::EES_Attacking;
    bCanDamage = true;
    // Play montage
    const float Duration = AnimInstance->Montage_Play(MeleeAttackMontage, 1.0f);
    
    if (Duration > 0.f)
    {
        // Bind to montage ended delegate
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, MeleeAttackMontage);
        UE_LOG(LogTemp, Log, TEXT("%s playing attack montage (duration: %.2f)"), *GetName(), Duration);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s failed to play attack montage!"), *GetName());
        EnemyState = EEnemyState::EES_Idle;
    }
}


void AEnemyBase::PlayRandomMeleeAttackMontage()
{
    if (!MeleeAttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no MeleeAttackMontage assigned!"), *GetName());
        PlayAttackMontage(); // Fallback to generic attack
        return;
    }
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;
    if (AnimInstance->IsAnyMontagePlaying())
    {
        return;
    }
    // Set state
    EnemyState = EEnemyState::EES_Attacking;
    bCanDamage = true;
    DamagedActors.Empty();
    // Pick random section
    const int32 NumSections = MeleeAttackMontage->CompositeSections.Num();
    int32 Selection = 0;
    
    if (NumSections > 0)
    {
        Selection = FMath::RandRange(0, NumSections - 1);
    }
    // Play montage
    const float Duration = AnimInstance->Montage_Play(MeleeAttackMontage, 1.0f);
    
    if (Duration > 0.f)
    {
        if (NumSections > 0)
        {
            const FName SectionName = MeleeAttackMontage->GetSectionName(Selection);
            AnimInstance->Montage_JumpToSection(SectionName, MeleeAttackMontage);
            UE_LOG(LogTemp, Log, TEXT("%s playing melee section: %s"), *GetName(), *SectionName.ToString());
        }
        // Bind end delegate
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, MeleeAttackMontage);
    }
    UE_LOG(LogTemp, Log, TEXT("%s random melee montage triggered."), *GetName());
}

void AEnemyBase::PlayRandomMajixAttackMontage()
{
    if (!MajixAttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no MajixAttackMontage assigned!"), *GetName());
        return;
    }
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;
    if (AnimInstance->IsAnyMontagePlaying())
    {
        return;
    }
    // Set state
    EnemyState = EEnemyState::EES_Attacking;
    // Pick random section
    const int32 NumSections = MajixAttackMontage->CompositeSections.Num();
    int32 Selection = 0;
    
    if (NumSections > 0)
    {
        Selection = FMath::RandRange(0, NumSections - 1);
    }
    // Play montage
    const float Duration = AnimInstance->Montage_Play(MajixAttackMontage, 1.0f);
    if (Duration > 0.f)
    {
        if (NumSections > 0)
        {
            const FName SectionName = MajixAttackMontage->GetSectionName(Selection);
            AnimInstance->Montage_JumpToSection(SectionName, MajixAttackMontage);
        }
        // Bind end delegate
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, MajixAttackMontage);
    }
    UE_LOG(LogTemp, Log, TEXT("%s random majix montage triggered."), *GetName());
}


void AEnemyBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Log, TEXT("%s attack montage ended. Interrupted: %d"), *GetName(), bInterrupted);
    // Reset attack state
    EnemyState = EEnemyState::EES_Idle;
    bCanDamage = false;
    DamagedActors.Empty();
    // Disable weapon collision
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    // If has combat target, resume pursuit
    if (CombatTarget && EnemyController)
    {
        if (!IsInsideAttackRadius())
        {
            EnemyController->MoveToActor(CombatTarget, AcceptanceRadius);
        }
    }
}

bool AEnemyBase::CanAttack()
{
    // Basic checks
    if (bDead || !CombatTarget)
    {
        return false;
    }
    // Check if already attacking
    if (EnemyState == EEnemyState::EES_Attacking)
    {
        return false;
    }
    // Check if in range
    if (!IsInsideAttackRadius())
    {
        return false;
    }
    // Check if animation is playing
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (AnimInstance->IsAnyMontagePlaying())
        {
            return false;
        }
    }
    return true;
}

void AEnemyBase::RegisterAttackCollision(UBoxComponent* CollisionBox)
{
    if (!CollisionBox) return;
    AttackCollisions.Add(CollisionBox);
}


// -----------------------------------------------------------------------------
// GAS / Attributes
// -----------------------------------------------------------------------------

UAbilitySystemComponent* AEnemyBase::GetEnemyASC_Implementation() const
{
    return EnemyAbilitySystemComponent;
}

UAttributeSet* AEnemyBase::GetEnemyAttributeSet_Implementation() const
{
    return EnemyAttributeSet;
}

void AEnemyBase::BroadcastEnemyAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
    if (!EnemyAttributeInfoOverride)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyAttributeInfo is nullptr in %s when broadcasting tag [%s]"),
            *GetName(), *AttributeTag.ToString());
        return;
    }
    if (!AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("AttributeSet is nullptr in %s when broadcasting tag [%s]"),
            *GetName(), *AttributeTag.ToString());
        return;
    }

    FHAFAttributeInfo Info = EnemyAttributeInfoOverride->FindAttributeInfoForTag(AttributeTag);
    Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
    EnemyAttributeInfoDelegate.Broadcast(Info);
}

void AEnemyBase::BindCallbacksToDependencies_Implementation()
{
    if (!AbilitySystemComponent || !AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing ASC or AttributeSet for delegate binding"), *GetName());
        return;
    }

    UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetHealthAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInHealth);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetShieldAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInShield);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetStaminaAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInStamina);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetMajixAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInMajix);}

void AEnemyBase::BroadcastInitialEnemyValues_Implementation()
{
    if (!AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing AttributeSet in BroadcastInitialEnemyValues"), *GetName());
        return;
    }

    UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet);
    OnEnemyHealthChanged.Broadcast(HAFAttSet->GetHealth());
    OnEnemyMaxHealthChanged.Broadcast(HAFAttSet->GetMaxHealth());
    OnEnemyShieldChanged.Broadcast(HAFAttSet->GetShield());
    OnEnemyMaxShieldChanged.Broadcast(HAFAttSet->GetMaxShield());
    OnEnemyStaminaChanged.Broadcast(HAFAttSet->GetStamina());
    OnEnemyMaxStaminaChanged.Broadcast(HAFAttSet->GetMaxStamina());
    OnEnemyMajixChanged.Broadcast(HAFAttSet->GetMajix());
    OnEnemyMaxMajixChanged.Broadcast(HAFAttSet->GetMaxMajix());
}

void AEnemyBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
    if (bHitReacting)
    {
        if (bIsCharmed)
        {
            if (AAIController* AIC = Cast<AAIController>(GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    BB->SetValueAsBool(TEXT("IsHitReacting"), true);
                }
            }
        }
    }
} 

void AEnemyBase::HandleChangeInHealth(const FOnAttributeChangeData& Data)
{
    OnEnemyHealthChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInMaxHealth(const FOnAttributeChangeData& Data)
{
    OnEnemyMaxHealthChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInShield(const FOnAttributeChangeData& Data)
{
    OnEnemyShieldChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInMaxShield(const FOnAttributeChangeData& Data)
{
    OnEnemyMaxShieldChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInStamina(const FOnAttributeChangeData& Data)
{
    OnEnemyStaminaChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInMaxStamina(const FOnAttributeChangeData& Data)
{
    OnEnemyMaxStaminaChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInMajix(const FOnAttributeChangeData& Data)
{
    OnEnemyMajixChanged.Broadcast(Data.NewValue);
}

void AEnemyBase::HandleChangeInMaxMajix(const FOnAttributeChangeData& Data)
{
    OnEnemyMaxMajixChanged.Broadcast(Data.NewValue);
}

// -----------------------------------------------------------------------------
// AI / Perception / Team
// -----------------------------------------------------------------------------

AHAFAIController* AEnemyBase::LaunchEnemyHAFAIController()
{
    return EnemyController;
}

void AEnemyBase::PawnSeen(APawn* SeenPawn)
{
    if (!SeenPawn || bDead) return;
    // Check if it's an enemy (player)
    if (SeenPawn->ActorHasTag(FName("Player")) || SeenPawn->ActorHasTag(FName("Enemy")))
    {
        if (!CombatTarget)
        {
            CombatTarget = SeenPawn;
            UE_LOG(LogTemp, Log, TEXT("%s acquired combat target: %s"), *GetName(), *GetNameSafe(SeenPawn));
        }
        // Update Blackboard if using Behavior Trees
        if (EnemyController)
        {
            if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
            {
                BlackboardComp->SetValueAsObject(FName("CombatTarget"), SeenPawn);
                BlackboardComp->SetValueAsBool(FName("HasTarget"), true);
            }
        }
        // Change state to chasing
        EnemyState = EEnemyState::EES_Chasing;
    }
}

void AEnemyBase::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || bDead) return;
    const bool bWasSuccessfullySensed = Stimulus.WasSuccessfullySensed();
    const FString StimulusText = bWasSuccessfullySensed ? TEXT("Sensed") : TEXT("Lost");
    
    UE_LOG(LogTemp, Log, TEXT("%s %s perception of %s"), *GetName(), *StimulusText, *GetNameSafe(Actor));
    if (bWasSuccessfullySensed)
    {
        // Target detected
        if (APawn* DetectedPawn = Cast<APawn>(Actor))
        {
            if (DetectedPawn->ActorHasTag(FName("Player")))
            {
                CombatTarget = DetectedPawn;
                
                // Update Blackboard
                if (EnemyController)
                {
                    if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
                    {
                        BlackboardComp->SetValueAsObject(FName("CombatTarget"), DetectedPawn);
                        BlackboardComp->SetValueAsVector(FName("TargetLocation"), DetectedPawn->GetActorLocation());
                        BlackboardComp->SetValueAsBool(FName("HasTarget"), true);
                    }
                }
                EnemyState = EEnemyState::EES_Chasing;
            }
        }
    }
    else
    {
        // Target lost
        if (Actor == CombatTarget)
        {
            // Update Blackboard
            if (EnemyController)
            {
                if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
                {
                    BlackboardComp->ClearValue(FName("CombatTarget"));
                    BlackboardComp->SetValueAsBool(FName("HasTarget"), false);
                }
            }
            CombatTarget = nullptr;
            EnemyState = EEnemyState::EES_Patrolling;
        }
    }
}

FGenericTeamId AEnemyBase::GetGenericTeamId() const
{
    return TeamId;
}

// -----------------------------------------------------------------------------
// Souls / Charm / Flee
// -----------------------------------------------------------------------------

void AEnemyBase::SpawnSoul()
{
    if (!SoulClass || !HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Cannot spawn soul (SoulClass: %s, HasAuthority: %d)"), 
            *GetName(), SoulClass ? TEXT("Valid") : TEXT("None"), HasAuthority());
        return;
    }
    const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // Slightly above ground
    const FRotator SpawnRotation = FRotator::ZeroRotator;
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ASoul* SpawnedSoul = GetWorld()->SpawnActor<ASoul>(
        SoulClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    if (SpawnedSoul)
    {
        // Optionally set soul value based on enemy level/type
        SpawnedSoul->SetSoulValue(SoulValue); // You'd need to add this property
        
        UE_LOG(LogTemp, Log, TEXT("%s spawned soul at %s"), *GetName(), *SpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s failed to spawn soul!"), *GetName());
    }
}

void AEnemyBase::TriggerCharm(AActor* InPlayerActor)
{
    CachedPlayer = InPlayerActor;
    bIsCharmed = true;
    UE_LOG(LogTemp, Log, TEXT("%s charmed by %s"), *GetName(), *GetNameSafe(InPlayerActor));
}

void AEnemyBase::BeginFlee()
{
    if (bIsFleeing) return;
    bIsFleeing = true;
    UE_LOG(LogTemp, Log, TEXT("%s fleeing!"), *GetName());
    DoNextFleeHop();
}

void AEnemyBase::DoNextFleeHop()
{
    if (!CachedPlayer) return;

    const FVector Dir = (GetActorLocation() - CachedPlayer->GetActorLocation()).GetSafeNormal();
    const FVector Dest = GetActorLocation() + Dir * FleeHopDistance;
    UE_LOG(LogTemp, Log, TEXT("%s hopping away to %s"), *GetName(), *Dest.ToString());

    SetActorLocation(Dest, true);
}

void AEnemyBase::AddStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s adding tag %s"), *GetName(), *Tag.ToString());
}

void AEnemyBase::RemoveStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s removing tag %s"), *GetName(), *Tag.ToString());
}

// -----------------------------------------------------------------------------
// Miscellaneous helpers
// -----------------------------------------------------------------------------

int32 AEnemyBase::GetPlayerLevel()
{
    return 1;
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UE_LOG(LogTemp, Log, TEXT("%s EndPlay cleanup."), *GetName());
    // --- Clean up hover UI immediately on destruction ---
    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->RemoveFromParent();
        ActiveAttributeMenuWidget = nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("%s EndPlay cleanup."), *GetName());
}

void AEnemyBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (!HasAuthority()) return;
    HAFAIController = Cast<AHAFAIController>(NewController);

    HAFAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    HAFAIController->RunBehaviorTree(BehaviorTree);
}

void AEnemyBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitializeAbilityActorInfo();
}

void AEnemyBase::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
    // Base class intentionally empty.
    // Subclasses (like AEnemyCombatBase) override to enable/disable melee hitboxes.}
}

void AEnemyBase::SafeInitASC_ForPawnOwner()
{
    if (EnemyAbilitySystemComponent)
    {
        EnemyAbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void AEnemyBase::HighlightActor()
{
    if (GetMesh())
    {
        GetMesh()->SetRenderCustomDepth(true);
        GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }

    if (EquippedEnemyRangedWeapon && EquippedEnemyRangedWeapon->GetWeaponMesh())
    {
        EquippedEnemyRangedWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
        EquippedEnemyRangedWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }

    if (EquippedEnemyMeleeWeapon && EquippedEnemyMeleeWeapon->GetWeaponMesh())
    {
        EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
        EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }

    if (EquippedEnemyWeapon && EquippedEnemyWeapon->GetWeaponMesh())
    {
        EquippedEnemyWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
        EquippedEnemyWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
    }
}

void AEnemyBase::UnHighlightActor()
{
    if (GetMesh())
    {
        GetMesh()->SetRenderCustomDepth(false);
    }

    if (EquippedEnemyWeapon && EquippedEnemyWeapon->GetWeaponMesh())
    {
        EquippedEnemyWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
    }

    if (EquippedEnemyMeleeWeapon && EquippedEnemyMeleeWeapon->GetWeaponMesh())
    {
        EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
    }

    if (EquippedEnemyRangedWeapon && EquippedEnemyRangedWeapon->GetWeaponMesh())
    {
        EquippedEnemyRangedWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
    }
}

bool AEnemyBase::IsAttacking() const // [Restored]
{
    return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemyBase::IsPatrolling() const // [Restored]
{
    return EnemyState == EEnemyState::EES_Patrolling;
}

void AEnemyBase::SetEnemyState(EEnemyState NewState) // [Restored]
{
    EnemyState = NewState;
    UE_LOG(LogTemp, Log, TEXT("%s EnemyState set to %s"), *GetName(), *UEnum::GetValueAsString(EnemyState));
}

void AEnemyBase::ResetCanDamage()
{
    // Base implementation does nothing.
    // Combat enemies override this to reset bCanDamage after applying damage.
}
