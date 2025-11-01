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
#include "Characters/CharacterClassInfo.h"
#include "HAFComponents/AttributeComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "UI/Widgets/EnemyProgressBarBaseWidget.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pickupable, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetMesh()->SetCollisionProfileName(TEXT("Enemy"));
    GetMesh()->SetCollisionObjectType(ECC_Mesh);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetGenerateOverlapEvents(false);
    
    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp"));
    
    // --- Ability System ---
    EnemyAbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    EnemyAbilitySystemComponent->SetIsReplicated(true);
    EnemyAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
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
    // New Health Bar Widget
    HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
    HealthBarWidgetComponent->SetupAttachment(RootComponent);
    HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidgetComponent->SetDrawAtDesiredSize(false);
    HealthBarWidgetComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    ShieldBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShieldBarWidgetComponent"));
    ShieldBarWidgetComponent->SetupAttachment(RootComponent);
    ShieldBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    ShieldBarWidgetComponent->SetDrawAtDesiredSize(false);
    ShieldBarWidgetComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    
    if (HealthBarWidgetComponent && HealthBarWidgetClass)
    {
        HealthBarWidgetComponent->SetWidgetClass(HealthBarWidgetClass);
    }
    if (ShieldBarWidgetComponent && ShieldBarWidgetClass)
    {
        ShieldBarWidgetComponent->SetWidgetClass(ShieldBarWidgetClass);
    }
    
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;
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

    if (!EnemyHealthWidgetController && EnemyHealthWidgetControllerClass)
    {
        EnemyHealthWidgetController = NewObject<UEnemyWidgetControllerBase>(this, EnemyHealthWidgetControllerClass);
        EnemyHealthWidgetController->Initialize(this);
    }
    if (!EnemyShieldWidgetController && EnemyShieldWidgetControllerClass)
    {
        EnemyShieldWidgetController = NewObject<UEnemyWidgetControllerBase>(this, EnemyShieldWidgetControllerClass);
        EnemyShieldWidgetController->Initialize(this);
    }
    
    // --- Initialize AI Perception ---
    if (AIPerceptionComponent && AIPerceptionComponent->OnTargetPerceptionUpdated.IsBound() == false)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBase::OnTargetDetected);
    }
    else if (!AIPerceptionComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: AIPerceptionComponent is null in BeginPlay!"), *GetName());
    }

    // Defensive sanity checks for new widget components
    if (!HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("HealthBarWidgetComponent"));
        HealthBarWidgetComponent->SetupAttachment(RootComponent);
        HealthBarWidgetComponent->RegisterComponent();
    }

    if (!ShieldBarWidgetComponent)
    {
        ShieldBarWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ShieldBarWidgetComponent"));
        ShieldBarWidgetComponent->SetupAttachment(RootComponent);
        ShieldBarWidgetComponent->RegisterComponent();
    }
    
    // --- GAS setup ---
    InitializeAbilityActorInfo();
    InitializeDefaultAttributes();
    FGameplayTagContainer OwnedTags;
    if (EnemyAbilitySystemComponent) 
    {
        EnemyAbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
        UE_LOG(LogTemp, Warning, TEXT("%s Gameplay Tags: %s"), *GetName(), *OwnedTags.ToString());
    }
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
    if (EnemyAbilitySystemComponent && EnemyAttributeSet)
    {
        float Health = EnemyAttributeSet->GetHealth();
        float MaxHealth = EnemyAttributeSet->GetMaxHealth();
        UE_LOG(LogTemp, Error, TEXT("🟢 %s SPAWNED - Health: %.1f / %.1f"), 
            *GetName(), Health, MaxHealth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("🔴 %s MISSING ASC OR ATTRIBUTESET AT SPAWN!"), *GetName());
    }
    if (EnemyAbilitySystemComponent)
    {
        UE_LOG(LogTemp, Log, TEXT("Enemy ASC Attribute Sets:"));
        for (UAttributeSet* Set : EnemyAbilitySystemComponent->GetSpawnedAttributes())
        {
            UE_LOG(LogTemp, Log, TEXT(" - %s"), *Set->GetName());
        }
    }
    if (EnemyAbilitySystemComponent)
    {
        FGameplayTagContainer OwnedTagsAgain;
        EnemyAbilitySystemComponent->GetOwnedGameplayTags(OwnedTagsAgain);
        UE_LOG(LogTemp, Log, TEXT("Target Tags: %s"), *OwnedTagsAgain.ToString());
    }
}

void AEnemyBase::InitializeDefaultAttributes() const
{
    Super::InitializeDefaultAttributes();
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsDead()) return;
    if (EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }
    
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
    // Defensive sanity checks for HealthBarWidgetComponent
    if (!HealthBarWidgetComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Missing HealthBarWidgetComponent — creating one dynamically."), *GetName());
        HealthBarWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("HealthBarWidgetComponent"));
        HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
        HealthBarWidgetComponent->RegisterComponent();
    }

    // Defensive sanity checks for ShieldBarWidgetComponent
    if (!ShieldBarWidgetComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Missing ShieldBarWidgetComponent — creating one dynamically."), *GetName());
        ShieldBarWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ShieldBarWidgetComponent"));
        ShieldBarWidgetComponent->SetupAttachment(GetRootComponent());
        ShieldBarWidgetComponent->RegisterComponent();
    }

    // Initialize Health Bar Widget
    HealthBarWidgetComponent->InitWidget();
    UUserWidget* HealthRawWidget = HealthBarWidgetComponent->GetUserWidgetObject();
    if (!HealthRawWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] HealthBarWidgetComponent has no UserWidgetObject!"), *GetName());
    }
    else if (UEnemyProgressBarBaseWidget* HealthWidget = Cast<UEnemyProgressBarBaseWidget>(HealthRawWidget))
    {
        HealthBarWidget = HealthWidget;

        // f (EnemyHealthWidgetController)
        // {
        HealthWidget->SetWidgetController(EnemyHealthWidgetController);
        UE_LOG(LogTemp, Log, TEXT("[%s] Successfully initialized HealthBarWidget with controller: %s"),
            *GetName(), *GetNameSafe(EnemyHealthWidgetController));
        // }
        if (!EnemyHealthWidgetController)
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] HealthWidget initialized but EnemyHealthWidgetController is nullptr!"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] HealthBarWidgetComponent UserWidget is not of class UHealthBarWidget (got: %s)"),
            *GetName(), *GetNameSafe(HealthRawWidget->GetClass()));
        }

        // Initialize Shield Bar Widget
        ShieldBarWidgetComponent->InitWidget();
        UUserWidget* ShieldRawWidget = ShieldBarWidgetComponent->GetUserWidgetObject();
        if (!ShieldRawWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] ShieldBarWidgetComponent has no UserWidgetObject!"), *GetName());
        }
        else if (UEnemyProgressBarBaseWidget* ShieldWidget = Cast<UEnemyProgressBarBaseWidget>(ShieldRawWidget))
        {
            ShieldBarWidget = ShieldWidget;

            // if (EnemyShieldWidgetController)
            // {
            ShieldWidget->SetWidgetController(EnemyShieldWidgetController);
            UE_LOG(LogTemp, Log, TEXT("[%s] Successfully initialized ShieldBarWidget with controller: %s"),
                *GetName(), *GetNameSafe(EnemyShieldWidgetController));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] ShieldWidget initialized but EnemyShieldWidgetController is nullptr!"), *GetName());
        }
    
        if (!EnemyShieldWidgetController)
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] ShieldBarWidgetComponent UserWidget is not of class UShieldBarWidget (got: %s)"),
            *GetName(), *GetNameSafe(ShieldRawWidget->GetClass()));
        }
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
    EnemyController = Cast<AHAFAIController>(GetController());
    MoveToTarget(PatrolTarget);
    SpawnEnemyWeapon();
    UE_LOG(LogTemp, Log, TEXT("Enemy %s initialized."), *GetName());
}

void AEnemyBase::SpawnEnemyWeapon()
{

    UWorld* World = GetWorld();
    if (World && BaseWeaponClass)
    {
        AWeaponBase* DefaultWeapon = World->SpawnActor<AWeaponBase>(BaseWeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
        EquippedWeapon = DefaultWeapon;
    }
}

void AEnemyBase::CheckPatrolTarget()
{
    // Simplified example
    if (!EnemyController) return;
    if (InTargetRange(ChoosePatrolTarget(), PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBase::PatrolTimerFinished, WaitTime);
    }
    UE_LOG(LogTemp, Verbose, TEXT("%s checking patrol target"), *GetName());
}

void AEnemyBase::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}

void AEnemyBase::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        ClearAttackTimer();
        EnemiesLoseInterest();
        if (!IsEnemyEngaged()) EnemiesStartPatrolling();
    }
    else if (IsOutsideAttackRadius() && !IsEnemyChasing())
    {
        ClearAttackTimer();
        if (!IsEnemyEngaged()) EnemiesChaseTarget();
    }
    else if (CanAttack())
    {
        StartAttackTimer();
    }
}

void AEnemyBase::EnemiesLoseInterest()
{
    CombatTarget = nullptr;
    HideEnemyStatWidgets();
}

void AEnemyBase::EnemiesStartPatrolling()
{
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    MoveToTarget(PatrolTarget);
}
void AEnemyBase::EnemiesChaseTarget()
{
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemyBase::IsOutsideCombatRadius() { return !InTargetRange(CombatTarget, CombatRadius); }
bool AEnemyBase::IsOutsideAttackRadius() { return !InTargetRange(CombatTarget, AttackRadius); }
bool AEnemyBase::IsInsideAttackRadius() { return InTargetRange(CombatTarget, AttackRadius); }
bool AEnemyBase::IsEnemyChasing() const { return EnemyState == EEnemyState::EES_Chasing; } 
bool AEnemyBase::IsEnemyAttacking() const { return EnemyState == EEnemyState::EES_Attacking; }
bool AEnemyBase::IsEnemyEngaged() const { return EnemyState == EEnemyState::EES_Engaged; }
bool AEnemyBase::IsEnemyDead() const { return EnemyState == EEnemyState::EES_Dead; }

void AEnemyBase::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemyBase::StartAttackTimer()
{
    EnemyState = EEnemyState::EES_Attacking;
    const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyBase::Attack, AttackTime);
}

void AEnemyBase::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemyBase::InTargetRange(AActor* Target, double Radius) const 
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}

void AEnemyBase::MoveToTarget(AActor* Target) const
{
    if (EnemyController == nullptr || Target == nullptr) return;
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemyBase::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for (AActor* Target : PatrolTargets)
    {
        if (Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }

    const int32 NumPatrolTargets = ValidTargets.Num();
    if (NumPatrolTargets > 0)
    {
        const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSelection];
    }
    return nullptr;
}


void AEnemyBase::AttackEnd()
{
    EnemyState = EEnemyState::EES_Idle;
    bCanDamage = false;
    DamagedActors.Empty();
    CheckCombatTarget();
    
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
    CombatTarget = EventInstigator->GetPawn();

    if (IsInsideAttackRadius())
    {
        EnemyState = EEnemyState::EES_Attacking;
    }
    else if (IsOutsideAttackRadius())
    {
        EnemiesChaseTarget();
    }
    
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
            UE_LOG(LogTemp, Warning, TEXT("✅ Applied GAS damage effect: %.1f"), DamageAmount);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s: No DamageEffectClass assigned!"), *GetName());
    }

    // Show hit react
    if (IsValid(DamageCauser))
    {
        GetHit_Implementation(GetActorLocation(), DamageCauser);
    }

    // Check for death
    const float CurrentHealth = EnemyAttributeSet->GetHealth();
    UE_LOG(LogTemp, Warning, TEXT("💚 %s current health: %.1f"), *GetName(), CurrentHealth);
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

    if (EquippedEnemyWeapon || EquippedEnemyMeleeWeapon || EquippedEnemyRangedWeapon)
    ClearPatrolTimer();
    ClearAttackTimer();
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

    if (IsInsideAttackRadius())
    {
        if (!IsDead()) StartAttackTimer();
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

void AEnemyBase::Destroyed()
{
    if (EquippedEnemyWeapon)
    {
        EquippedEnemyWeapon->Destroy();
    }
    if (EquippedEnemyMeleeWeapon)
    {
        EquippedEnemyMeleeWeapon->Destroy();
    }
    if (EquippedEnemyRangedWeapon)
    {
        EquippedEnemyRangedWeapon->Destroy();
    }
}
void AEnemyBase::Die()
{
    if (bDead) return;
    bDead = true;

    UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());

    if (EnemyController && EnemyController->BrainComponent)
    {
        EnemyController->BrainComponent->StopLogic(TEXT("Enemy Died"));
        if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
        {
            BlackboardComp->ClearValue(FName("CombatTarget"));
            BlackboardComp->SetValueAsBool(FName("IsDead"), true);
        }   
    }
    
    // Immediately stop AI movement and blackboard logic
    if (EnemyController)
    {
        EnemyController->StopMovement();

        if (UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent())
        {
            BlackboardComp->ClearValue(FName("CombatTarget"));
            BlackboardComp->SetValueAsBool(FName("IsDead"), true);
        }

        if (EnemyController->BrainComponent)
        {
            EnemyController->BrainComponent->StopLogic(TEXT("Enemy died"));
        }
    }

    // Stop character movement and tick to avoid further pathfinding updates
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
        MoveComp->SetComponentTickEnabled(false);
    }

    if (EnemyController)
    {
        EnemyController->StopMovement();
    }

    // Disable collisions to avoid further triggers
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    // Clean up UI widgets
    if (HealthBarWidget) HealthBarWidget->RemoveFromParent();
    if (ShieldBarWidget) ShieldBarWidget->RemoveFromParent();
    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->RemoveFromParent();
        ActiveAttributeMenuWidget = nullptr;
    }
    
    ClearAttackTimer();

    // Play death animation, dissolve, spawn soul as usual
    PlayDeathMontage();
    MulticastHandleDeath_Implementation();
    Dissolve();
    SpawnSoul();

    // Destroy actor after a delay to let effects finish
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

void AEnemyBase::HideEnemyStatWidgets()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (ShieldBarWidget)
    {
        ShieldBarWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AEnemyBase::ShowEnemyStatWidgets()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(ESlateVisibility::Visible);
    }
    if (ShieldBarWidget)
    {
        ShieldBarWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEnemyBase::Attack()
{
    if (EquippedMeleeWeapon) MeleeAttack();
    if (EquippedRangedWeapon) RangedAttack();
}

void AEnemyBase::MeleeAttack()
{
    Super::MeleeAttack();
	
    if (!CanAttack() || !CombatTarget) return;

    SetEnemyState(EEnemyState::EES_Attacking);
    bCanDamage = true;
    DamagedActors.Empty();

    SetWarpTargetsForCombatTarget(CombatTarget);

    if (MeleeAttackMontage && GetMesh())
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            const float Duration = AnimInstance->Montage_Play(MeleeAttackMontage, 1.0f);
            if (Duration > 0.f)
            {
                // ✅ Correct delegate binding for UE 5.5.4
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
                AnimInstance->Montage_SetEndDelegate(EndDelegate);
            }
        }
    }
}

void AEnemyBase::MajixAttack()
{
    if (!CanAttack()) return;
    UE_LOG(LogTemp, Log, TEXT("%s performing majix attack."), *GetName());
        PlayRandomMajixAttackMontage();
}

void AEnemyBase::RangedAttack()
{
    if (EquippedRangedWeapon)
    {
        UCombatComponent* EnemyCombatComponent = NewObject<UCombatComponent>(this, CombatComponentClass);
        EnemyCombatComponent->SetIsReplicated(true);
        EnemyCombatComponent->Fire();
    }
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
    bCanDamage = true;
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

void AEnemyBase::RegisterAttackCollision(UBoxComponent* CollisionBox)
{
    if (!CollisionBox) return;

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionObjectType(ECC_Pawn);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CollisionBox->SetGenerateOverlapEvents(true);
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnAttackCollisionOverlap);

    AttackCollisions.AddUnique(CollisionBox);
    
    UE_LOG(LogTemp, Log, TEXT("%s registered attack collision: %s"), *GetName(), *CollisionBox->GetName());
}

void AEnemyBase::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
    for (UBoxComponent* Box : AttackCollisions)
    {
        if (!Box) continue;
        Box->SetCollisionEnabled(CollisionEnabled);
        Box->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);
    }

    if (CollisionEnabled == ECollisionEnabled::NoCollision)
    {
        DamagedActors.Empty();
    }
    
    UE_LOG(LogTemp, Log, TEXT("%s set weapon collision: %s"), 
        *GetName(), 
        CollisionEnabled == ECollisionEnabled::QueryOnly ? TEXT("ENABLED") : TEXT("DISABLED"));
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
        return EnemyAbilitySystemComponent;
}

void AEnemyBase::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("⚔️ %s attack collision overlap with %s"), 
       *GetName(), *GetNameSafe(OtherActor));
    
    if (!bCanDamage || !OtherActor || OtherActor == this) return;

    if (DamagedActors.Contains(OtherActor)) return;
    
    DamagedActors.Add(OtherActor);

    // Apply damage
    UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetController(), this, nullptr);
    
    UE_LOG(LogTemp, Warning, TEXT("💥 %s damaged %s for %.1f!"), 
        *GetName(), *GetNameSafe(OtherActor), BaseDamage);

    // Reset damage cooldown
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.25f, false);
}

void AEnemyBase::ResetCanDamage()
{
    bCanDamage = true;
    DamagedActors.Empty();
}



bool AEnemyBase::CanAttack()
{
    bool bCanAttack = IsInsideAttackRadius() &&
        !IsAttacking() &&
            !IsEnemyEngaged() &&
                !IsDead();
    return bCanAttack;
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
    if (!EnemyAbilitySystemComponent || !EnemyAttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing ASC or AttributeSet for delegate binding"), *GetName());
        return;
    }

    UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(EnemyAttributeSet);
   EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetHealthAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInHealth);

    EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetShieldAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInShield);

    EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetStaminaAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInStamina);

    EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetMajixAttribute())
        .AddUObject(this, &AEnemyBase::HandleChangeInMajix);}

void AEnemyBase::BroadcastInitialEnemyValues_Implementation()
{
    if (!EnemyAttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing AttributeSet in BroadcastInitialEnemyValues"), *GetName());
        return;
    }

    UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(EnemyAttributeSet);
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
    HAFAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);

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
    UE_LOG(LogTemp, Error, TEXT("❤️ %s Health changed: %.1f → %.1f"), 
       *GetName(), Data.OldValue, Data.NewValue);
    OnEnemyHealthChanged.Broadcast(Data.NewValue);

    if (Data.NewValue <= 0.0f && !bDead)
    {
        Die();
    }
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
    }

    const bool bShouldChaseTarget = EnemyState != EEnemyState::EES_Dead && EnemyState != EEnemyState::EES_Chasing && EnemyState < EEnemyState::EES_Attacking && (SeenPawn->ActorHasTag(FName("EngageableTarget")) || SeenPawn->ActorHasTag(FName("Player")));

    if (bShouldChaseTarget)
    {
        CombatTarget = SeenPawn;
        ClearPatrolTimer();
        EnemiesChaseTarget();
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

void AEnemyBase::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || bDead) return;
    const bool bWasSuccessfullySensed = Stimulus.WasSuccessfullySensed();
    const FString StimulusText = bWasSuccessfullySensed ? TEXT("Sensed") : TEXT("Lost");
    
    // UE_LOG(LogTemp, Log, TEXT("%s %s perception of %s"), *GetName(), *StimulusText, *GetNameSafe(Actor));
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
    UWorld* World = GetWorld();
    if (World && SoulClass && AttributeComponent)
    {
        const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // Slightly above ground
        const FRotator SpawnRotation = FRotator::ZeroRotator;
        ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, SpawnRotation);
        if (SpawnedSoul)
        {
            SpawnedSoul->SetSoulValue(AttributeComponent->GetSoulsGathered());
            SpawnedSoul->SetOwner(this);
            UE_LOG(LogTemp, Log, TEXT("%s spawned soul at %s"), *GetName(), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("%s failed to spawn soul!"), *GetName());
        }
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
    
    // ✅ Safety checks
    if (!HAFAIController)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: NewController is not HAFAIController!"), *GetName());
        return;
    }
    
    if (!BehaviorTree)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: No BehaviorTree assigned!"), *GetName());
        return;
    }
    
    // ✅ Use UseBlackboard to safely initialize
    UBlackboardComponent* BlackboardComp = nullptr;
    if (HAFAIController->UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComp))
    {
        UE_LOG(LogTemp, Log, TEXT("%s: Blackboard initialized successfully"), *GetName());
        HAFAIController->RunBehaviorTree(BehaviorTree);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to initialize Blackboard!"), *GetName());
    }

    HAFAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
    HAFAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), (CharacterClass == ECharacterClass::Gunslinger || CharacterClass == ECharacterClass::Majixian));
}


void AEnemyBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitializeAbilityActorInfo();
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

