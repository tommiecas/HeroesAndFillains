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
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Characters/CharacterClassInfo.h"
#include "Characters/FillainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HAFComponents/AttributeComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "UI/Widgets/EnemyProgressBarBaseWidget.h"
#include "Interfaces/HitInterface.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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
    
    // --- Ability System ---
    EnemyAbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    EnemyAbilitySystemComponent->SetIsReplicated(true);
    EnemyAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    EnemyAttributeSet = CreateDefaultSubobject<UHAFAttributeSet>(TEXT("AttributeSet"));
    
    // Set base class pointers so BaseCharacter methods can access them
    AbilitySystemComponent = EnemyAbilitySystemComponent;
    AttributeSet = EnemyAttributeSet;

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

    if (HasAuthority()) UHAFAbilitySystemBlueprintLibrary::GiveStartupAbilities(this, EnemyAbilitySystemComponent, CharacterClass);
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

        InitializeDefaultAttributes();

        if (EnemyAttributeSet)
        {
            UE_LOG(LogTemp, Error, TEXT("AFTER ATTR INIT: Health=%.1f  MaxHealth=%.1f"),
                EnemyAttributeSet->GetHealth(),
                EnemyAttributeSet->GetMaxHealth());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AFTER ATTR INIT: EnemyAttributeSet is NULL!"));
        }

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
        // UE_LOG(LogTemp, Error, TEXT("🟢 %s SPAWNED - Health: %.1f / %.1f"), 
        //    *GetName(), Health, MaxHealth);
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
    InitializeEnemy();
}

void AEnemyBase::InitializeDefaultTags()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.EnemyAI")));
    }
}

void AEnemyBase::InitializeDefaultAttributes()
{
    Super::InitializeDefaultAttributes();
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Execute_IsDead(this)) return;
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
    if (bIsHovered) return;
    bIsHovered = true;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !EnemyAttributeMenuWidgetClass)
        return;

    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(PC, EnemyAttributeMenuWidgetClass);
    if (!ActiveAttributeMenuWidget) return;

    ActiveAttributeMenuWidget->AddToViewport(10);
    ActiveAttributeMenuWidget->SetWidgetController(EnemyWidgetController);
    ActiveAttributeMenuWidget->FadeIn(0.2f);

    FVector2D MousePos;
    PC->GetMousePosition(MousePos.X, MousePos.Y);
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
    SpawnEnemyWeapon();
    UE_LOG(LogTemp, Log, TEXT("Enemy %s initialized."), *GetName());
}

void AEnemyBase::SpawnEnemyWeapon_Implementation()
{
    UWorld* World = GetWorld();
    if (World && BaseWeaponClass)
    {
        // Check if BaseWeaponClass is a RangedWeapon subclass
        if (BaseWeaponClass->IsChildOf(ARangedWeapon::StaticClass()))
        {
            ARangedWeapon* DefaultWeapon = World->SpawnActor<ARangedWeapon>(BaseWeaponClass);
            if (DefaultWeapon)
            {
                EquippedWeapon = DefaultWeapon;
                EquippedEnemyRangedWeapon = DefaultWeapon;
                EquippedWeapon->Equip(GetMesh(), FName("RangedSocket"), this, this);
            }
        }
        // Check if BaseWeaponClass is a MeleeWeapon subclass
        else if (BaseWeaponClass->IsChildOf(AMeleeWeapon::StaticClass()))
        {
            AMeleeWeapon* DefaultWeapon = World->SpawnActor<AMeleeWeapon>(BaseWeaponClass);
            if (DefaultWeapon)
            {
                EquippedWeapon = DefaultWeapon;
                EquippedEnemyMeleeWeapon = DefaultWeapon;
                DefaultWeapon->Equip(GetMesh(), FName("MeleeSocket"), this, this);
                
            }
        }
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
    // Don't start a new timer if one is already active
    if (GetWorldTimerManager().IsTimerActive(AttackTimer))
    {
        return;
    }
    
    // Don't set state to Attacking yet - let Attack() do that after CanAttack() check
    const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyBase::Attack, AttackTime);
    UE_LOG(LogTemp, Warning, TEXT("⏰ %s starting attack timer (%.2fs)"), *GetName(), AttackTime);
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
    // ✅ Prevent damage after death
    if (bDead || Execute_IsDead(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ %s is already dead, ignoring damage"), *GetName());
        return;
    }
    
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
        if (!Execute_IsDead(this)) StartAttackTimer();
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
    // ❌ REMOVED: SetLifeSpan(3.0f) - This was conflicting with Die()'s SetLifeSpan(5.0f)
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

void AEnemyBase::Die_Implementation()
{
    if (bDead) return;
    bDead = true;
    EnemyState = EEnemyState::EES_Dead;

    UE_LOG(LogTemp, Log, TEXT("%s died."), *GetName());

    // --- SHUT DOWN AI CLEANLY BEFORE ANYTHING ELSE ---
    if (EnemyController)
    {
        // Stop any movement
        EnemyController->StopMovement();

        // Stop BT / EQS / AI logic
        if (EnemyController->BrainComponent)
        {
            EnemyController->BrainComponent->StopLogic(TEXT("Enemy died"));
            EnemyController->BrainComponent->SetComponentTickEnabled(false);
        }

        // Stop path following tick (THIS is what caused your crash)
        if (UPathFollowingComponent* PathComp = EnemyController->GetPathFollowingComponent())
        {
            PathComp->SetComponentTickEnabled(false);
        }

        // Disable blackboard
        if (UBlackboardComponent* BB = EnemyController->GetBlackboardComponent())
        {
            BB->ClearValue(FName("CombatTarget"));
            BB->SetValueAsBool(FName("IsDead"), true);
        }

        // Fully detach the controller
        EnemyController->UnPossess();

        // Stop controller ticking
        EnemyController->SetActorTickEnabled(false);

        // DESTROY controller so it cannot tick next frame
        EnemyController->Destroy();
        EnemyController = nullptr;
    }

    // --- STOP MOVEMENT COMPONENT ---
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
        MoveComp->SetComponentTickEnabled(false);
    }

    // --- COLLISION CLEANUP ---
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Ignore);

    // --- WIDGET CLEANUP ---
    if (HealthBarWidget) HealthBarWidget->RemoveFromParent();
    if (ShieldBarWidget) ShieldBarWidget->RemoveFromParent();
    if (ActiveAttributeMenuWidget)
    {
        ActiveAttributeMenuWidget->RemoveFromParent();
        ActiveAttributeMenuWidget = nullptr;
    }

    ClearAttackTimer();

    // --- PLAY DEATH MONTAGE AND FREEZE WHEN DONE ---
    const int32 Section = PlayDeathMontage();

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (DeathMontage)
        {
            FOnMontageEnded EndDelegate;
            EndDelegate.BindLambda([this](UAnimMontage*, bool)
            {
                if (GetMesh())
                {
                    GetMesh()->SetAnimInstanceClass(nullptr);
                    GetMesh()->bPauseAnims = true;
                    GetMesh()->bNoSkeletonUpdate = true;
                    GetMesh()->SetSimulatePhysics(false);
                    GetMesh()->SetComponentTickEnabled(false);
                }

                if (UCharacterMovementComponent* Move = GetCharacterMovement())
                {
                    Move->StopMovementImmediately();
                    Move->DisableMovement();
                    Move->SetComponentTickEnabled(false);
                }

                SetActorTickEnabled(false);
            });

            AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
        }
    }

    MulticastHandleDeath_Implementation();
    Dissolve();
    SpawnSoul();

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
        UE_LOG(LogTemp, Error, TEXT("❌ %s has no DeathMontage assigned!"), *GetName());
        return -1;
    }
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s has no AnimInstance!"), *GetName());
        return -1;
    }
    
    // Pick random death animation
    const int32 NumSections = DeathMontage->CompositeSections.Num();
    if (NumSections == 0)
    {
        const float Duration = AnimInstance->Montage_Play(DeathMontage);
        UE_LOG(LogTemp, Warning, TEXT("💀 %s C++ PlayDeathMontage (no sections), Duration: %.2f"), *GetName(), Duration);
        return 0;
    }
    if (NumSections == 1)
    {
        AnimInstance->Montage_Play(DeathMontage);
        UE_LOG(LogTemp, Warning, TEXT("💀 %s C++ Singular Death Montage Played"), *this->GetEnemyDisplayName().ToString());
        return NumSections;
    }
    if (NumSections > 1)
    {
        const int32 Selection = FMath::RandRange(0, NumSections - 1);
        const FName SectionName = DeathMontage->GetSectionName(Selection);
        const float Duration = AnimInstance->Montage_Play(DeathMontage);
        AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
    
        UE_LOG(LogTemp, Warning, TEXT("💀 %s C++ PlayDeathMontage section %d: %s, Duration: %.2f"), 
            *GetName(), Selection, *SectionName.ToString(), Duration);
    
        return Selection;
    }
    UE_LOG(LogTemp, Warning, TEXT("💀 No Death Montage"));
    return -1;
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
    UE_LOG(LogTemp, Warning, TEXT("🎯 %s Attack() called!"), *GetName());
    UE_LOG(LogTemp, Warning, TEXT("   EquippedMeleeWeapon: %s"), EquippedMeleeWeapon ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("   EquippedRangedWeapon: %s"), EquippedRangedWeapon ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("   MeleeAttackMontage: %s"), MeleeAttackMontage ? TEXT("YES") : TEXT("NO"));
    
    // If has ranged weapon, do ranged attack
    if (EquippedRangedWeapon)
    {
        RangedAttack();
        return;
    }
    
    // Otherwise do melee attack (works for both weapon-based and unarmed enemies like Gnarledlings)
    if (MeleeAttackMontage)
    {
        MeleeAttack();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s has no MeleeAttackMontage assigned!"), *GetName());
    }
}

void AEnemyBase::MeleeAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("🗡️ %s MeleeAttack() called!"), *GetName());
    
    Super::MeleeAttack();
	
    if (!CanAttack())
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ %s CanAttack() returned false"), *GetName());
        return;
    }
    
    if (!CombatTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ %s has no CombatTarget"), *GetName());
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("✅ %s passed CanAttack checks, calling PlayRandomMeleeAttackMontage"), *GetName());
    
    // Use the proper random montage function instead of playing MeleeAttackMontage directly
    PlayRandomMeleeAttackMontage();
}

void AEnemyBase::MajixAttack()
{
    if (!CanAttack()) return;
    UE_LOG(LogTemp, Log, TEXT("%s performing majix attack."), *GetName());
        PlayRandomMajixAttackMontage();
}

void AEnemyBase::RangedAttack()
{
    if (!CanFireWeapon())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s cannot fire weapon"), *GetName());
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("🔫 %s initiating ranged attack"), *GetName());
    
    // Fire the weapon
    FireWeapon();
    
    // Start cooldown timer (3 seconds reload)
    bCanFire = false;
    GetWorldTimerManager().SetTimer(
        FireCooldownTimer,
        this,
        &AEnemyBase::ResetFireCooldown,
        FireCooldownTime,
        false
    );
    
    UE_LOG(LogTemp, Log, TEXT("🔫 %s fired weapon, cooldown started (%.1fs)"), *GetName(), FireCooldownTime);
}

bool AEnemyBase::CanFireWeapon() const
{
    // Check if weapon exists
    if (!EquippedEnemyRangedWeapon)
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: No ranged weapon equipped"), *GetName());
        return false;
    }
    
    // Check if can fire (cooldown)
    if (!bCanFire)
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: Weapon on cooldown"), *GetName());
        return false;
    }
    
    // Check if has combat target
    if (!CombatTarget)
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: No combat target"), *GetName());
        return false;
    }
    
    // Check if in range
    const float DistanceToTarget = FVector::Dist(GetActorLocation(), CombatTarget->GetActorLocation());
    if (DistanceToTarget > MaxFiringRange)
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: Target out of range (%.1f > %.1f)"), 
            *GetName(), DistanceToTarget, MaxFiringRange);
        return false;
    }
    
    // Check line of sight
    if (!HasLineOfSight(CombatTarget))
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: No line of sight to target"), *GetName());
        return false;
    }
    
    return true;
}

bool AEnemyBase::HasLineOfSight(AActor* Target) const
{
    if (!Target) return false;
    
    UWorld* World = GetWorld();
    if (!World) return false;
    
    // Get weapon muzzle location if available, otherwise use actor location
    FVector StartLocation = GetActorLocation();
    if (EquippedEnemyRangedWeapon && EquippedEnemyRangedWeapon->GetWeaponMesh())
    {
        const USkeletalMeshSocket* MuzzleSocket = EquippedEnemyRangedWeapon->GetWeaponMesh()->GetSocketByName(FName("MuzzleFlashSocket"));
        if (MuzzleSocket)
        {
            StartLocation = MuzzleSocket->GetSocketLocation(EquippedEnemyRangedWeapon->GetWeaponMesh());
        }
    }
    
    // Aim at target's center mass (chest height)
    FVector TargetLocation = Target->GetActorLocation();
    if (APawn* TargetPawn = Cast<APawn>(Target))
    {
        // Aim at chest height for better hit chance
        TargetLocation.Z += 50.0f;
    }
    
    // Perform line trace
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(EquippedEnemyRangedWeapon);
    QueryParams.bTraceComplex = false;
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    // If we hit something, check if it's the target or if nothing is blocking
    if (bHit)
    {
        // If we hit the target directly, we have line of sight
        if (HitResult.GetActor() == Target)
        {
            return true;
        }
        
        // If we hit something else, no line of sight
        UE_LOG(LogTemp, Verbose, TEXT("%s: Line of sight blocked by %s"), 
            *GetName(), *GetNameSafe(HitResult.GetActor()));
        return false;
    }
    
    // No hit means clear line of sight
    return true;
}

FVector AEnemyBase::CalculateAimTarget() const
{
    if (!CombatTarget)
    {
        return GetActorForwardVector() * 1000.0f + GetActorLocation();
    }
    
    // Get target's location (aim at chest height)
    FVector TargetLocation = CombatTarget->GetActorLocation();
    if (APawn* TargetPawn = Cast<APawn>(CombatTarget))
    {
        TargetLocation.Z += 50.0f; // Chest height
    }
    
    // Calculate base direction to target
    FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
    
    // Add accuracy variation (inaccuracy increases with distance)
    const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetLocation);
    const float DistanceFactor = FMath::Clamp(DistanceToTarget / MaxFiringRange, 0.0f, 1.0f);
    
    // Calculate spread based on accuracy (lower accuracy = more spread)
    // At 70% accuracy, max spread is about 30 degrees at max range
    const float MaxSpreadDegrees = (1.0f - WeaponAccuracy) * 30.0f * DistanceFactor;
    const float SpreadRadians = FMath::DegreesToRadians(MaxSpreadDegrees);
    
    // Add random offset within spread cone
    const float RandomYaw = FMath::FRandRange(-SpreadRadians, SpreadRadians);
    const float RandomPitch = FMath::FRandRange(-SpreadRadians, SpreadRadians);
    
    // Apply rotation to direction
    FRotator DirectionRotation = DirectionToTarget.Rotation();
    DirectionRotation.Yaw += FMath::RadiansToDegrees(RandomYaw);
    DirectionRotation.Pitch += FMath::RadiansToDegrees(RandomPitch);
    
    // Calculate final aim point far in the distance
    const FVector AimDirection = DirectionRotation.Vector();
    const FVector AimTarget = GetActorLocation() + (AimDirection * 10000.0f); // Far point for trace
    
    UE_LOG(LogTemp, Verbose, TEXT("%s: Aim spread = %.2f degrees (accuracy: %.2f, distance: %.1f)"), 
        *GetName(), MaxSpreadDegrees, WeaponAccuracy, DistanceToTarget);
    
    return AimTarget;
}

void AEnemyBase::FireWeapon()
{
    if (!EquippedEnemyRangedWeapon || !CombatTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Cannot fire - missing weapon or target"), *GetName());
        return;
    }
    
    // Calculate aim target with accuracy variation
    FVector AimTarget = CalculateAimTarget();
    
    // Make enemy face the target
    FVector DirectionToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToTarget.Rotation();
    TargetRotation.Pitch = 0.0f; // Keep enemy upright
    SetActorRotation(TargetRotation);
    
    // Fire the weapon
    EquippedEnemyRangedWeapon->Fire(AimTarget);
    
    UE_LOG(LogTemp, Warning, TEXT("💥 %s FIRED at %s!"), 
        *GetName(), *GetNameSafe(CombatTarget));
}

void AEnemyBase::ResetFireCooldown()
{
    bCanFire = true;
    UE_LOG(LogTemp, Log, TEXT("🔫 %s weapon reloaded, ready to fire"), *GetName());
}

void AEnemyBase::PlayRandomMeleeAttackMontage()
{
    if (!MeleeAttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has no MeleeAttackMontage assigned!"), *GetName());
        return;
    }
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;
    
    // ✅ Clear any stale delegates first to prevent immediate callback
    FOnMontageEnded ClearDelegate;
    AnimInstance->Montage_SetEndDelegate(ClearDelegate, MeleeAttackMontage);
    
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
    
    // Play montage (will interrupt any currently playing animation)
    const float Duration = AnimInstance->Montage_Play(MeleeAttackMontage, 1.0f);
    
    UE_LOG(LogTemp, Warning, TEXT("🥊 %s PlayRandomMeleeAttackMontage - Duration: %.2f"), *GetName(), Duration);
    
    if (Duration > 0.f)
    {
        if (NumSections > 0)
        {
            const FName SectionName = MeleeAttackMontage->GetSectionName(Selection);
            AnimInstance->Montage_JumpToSection(SectionName, MeleeAttackMontage);
            UE_LOG(LogTemp, Warning, TEXT("   Playing section: %s"), *SectionName.ToString());
        }
        
        // ✅ Now bind the fresh delegate
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, MeleeAttackMontage);
        
        UE_LOG(LogTemp, Warning, TEXT("✅ %s ATTACK MONTAGE STARTED! Should run for %.2fs"), *GetName(), Duration);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ %s Montage_Play returned 0!"), *GetName());
        EnemyState = EEnemyState::EES_Idle;
    }
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
    
    // ✅ Clear any stale delegates first
    FOnMontageEnded ClearDelegate;
    AnimInstance->Montage_SetEndDelegate(ClearDelegate, MajixAttackMontage);
    
    // Set state
    EnemyState = EEnemyState::EES_Attacking;
    
    // Pick random section
    const int32 NumSections = MajixAttackMontage->CompositeSections.Num();
    int32 Selection = 0;
    
    if (NumSections > 0)
    {
        Selection = FMath::RandRange(0, NumSections - 1);
    }
    
    // Play montage (will interrupt any currently playing animation)
    const float Duration = AnimInstance->Montage_Play(MajixAttackMontage, 1.0f);
    
    if (Duration > 0.f)
    {
        if (NumSections > 0)
        {
            const FName SectionName = MajixAttackMontage->GetSectionName(Selection);
            AnimInstance->Montage_JumpToSection(SectionName, MajixAttackMontage);
        }
        
        // ✅ Now bind the fresh delegate
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &AEnemyBase::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, MajixAttackMontage);
        
        UE_LOG(LogTemp, Warning, TEXT("✅ %s majix attack montage started!"), *GetName());
    }
    else
    {
        EnemyState = EEnemyState::EES_Idle;
    }
}


void AEnemyBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("🏁 %s attack montage ended. Interrupted: %d"), *GetName(), bInterrupted);
    
    // Reset to Chasing (not Idle!) so CheckCombatTarget() continues to run in Tick()
    EnemyState = EEnemyState::EES_Chasing;
    bCanDamage = false;
    DamagedActors.Empty();

    // Disable weapon collision
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

    UE_LOG(LogTemp, Warning, TEXT("   State set to Chasing, CheckCombatTarget will handle next attack"));
}

void AEnemyBase::RegisterAttackCollision(UBoxComponent* CollisionBox)
{
    if (!CollisionBox) return;

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
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

void AEnemyBase::OnAttackCollisionOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("⚔️ %s attack collision overlap with %s"), 
        *GetName(), *GetNameSafe(OtherActor));

    if (!bCanDamage || !OtherActor || OtherActor == this) return;

    // ⭐ NEW: Don't damage dead actors
    if (OtherActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
    {
        if (ICombatInterface::Execute_IsDead(OtherActor))
        {
            UE_LOG(LogTemp, Warning, TEXT("💀 %s ignored dead target %s"), 
                *GetName(), *GetNameSafe(OtherActor));
            return;
        }
    }

    if (DamagedActors.Contains(OtherActor)) return;
    DamagedActors.Add(OtherActor);

    // GAS damage (unchanged)
    if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(OtherActor))
    {
        if (UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent())
        {
            if (DamageEffectClass && EnemyAbilitySystemComponent)
            {
                FGameplayEffectContextHandle EffectContext = 
                    EnemyAbilitySystemComponent->MakeEffectContext();
                EffectContext.AddSourceObject(this);
                EffectContext.AddInstigator(this, this);

                FGameplayEffectSpecHandle SpecHandle = 
                    EnemyAbilitySystemComponent->MakeOutgoingSpec(
                        DamageEffectClass, 1.0f, EffectContext);

                if (SpecHandle.IsValid())
                {
                    SpecHandle.Data->SetSetByCallerMagnitude(
                        FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
                        BaseDamage
                    );

                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                    UE_LOG(LogTemp, Warning, TEXT("💥 %s damaged %s for %.1f through GAS!"), 
                        *GetName(), *GetNameSafe(OtherActor), BaseDamage);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("❌ %s: No DamageEffectClass assigned!"), *GetName());
            }
        }
    }
    else
    {
        UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetController(), this, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("⚠️ %s used fallback damage on %s"),
            *GetName(), *GetNameSafe(OtherActor));
    }

    bCanDamage = false;
    GetWorldTimerManager().SetTimer(
        DamageResetTimer,
        this,
        &AEnemyBase::ResetCanDamage,
        0.25f,
        false
    );
}

void AEnemyBase::ResetCanDamage()
{
    bCanDamage = true;
    DamagedActors.Empty();
}



bool AEnemyBase::CanAttack()
{
    const bool bInRadius = IsInsideAttackRadius();
    const bool bNotEngaged = !IsEnemyEngaged();
    const bool bNotDead = !Execute_IsDead(this);
    const bool bCanAttack = bInRadius && bNotEngaged && bNotDead;
    
    UE_LOG(LogTemp, Warning, TEXT("🔍 %s CanAttack() check:"), *GetName());
    UE_LOG(LogTemp, Warning, TEXT("   IsInsideAttackRadius: %s"), bInRadius ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("   !IsEnemyEngaged: %s (State=%s)"), bNotEngaged ? TEXT("YES") : TEXT("NO"), *UEnum::GetValueAsString(EnemyState));
    UE_LOG(LogTemp, Warning, TEXT("   !IsDead: %s"), bNotDead ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("   → RESULT: %s"), bCanAttack ? TEXT("CAN ATTACK") : TEXT("CANNOT ATTACK"));
    
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
        if (AAIController* AIC = Cast<AAIController>(GetController()))
        {
            if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
            {
                BB->SetValueAsBool(TEXT("IsHitReacting"), true);
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
    if (AFillainCharacter* FC = Cast<AFillainCharacter>(CombatTarget))
    {
        FC->EnterCombat();
    }
}

void AEnemyBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        SetCombatTarget(Actor);
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

void AEnemyBase::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
    CombatTarget = InCombatTarget;
}

AActor* AEnemyBase::GetCombatTarget_Implementation()
{
    return CombatTarget;
}

void AEnemyBase::SetEnemyState(EEnemyState NewState) // [Restored]
{
    EnemyState = NewState;
    UE_LOG(LogTemp, Log, TEXT("%s EnemyState set to %s"), *GetName(), *UEnum::GetValueAsString(EnemyState));
}

