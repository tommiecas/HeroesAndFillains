// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include <Items/Soul.h>
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAFComponents/AttributeComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Weapons/WeaponBase.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HUD/Widgets/EnemyHealthBarWidget.h"
#include "HUD/Widgets/EnemyProgressBarBaseWidget.h"
#include "HUD/Widgets/HAFUserWidget.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "HAFGameplayTags.h"


static FGameplayTag TAG_State_Charmed = FGameplayTag::RequestGameplayTag(FName("State.Charmed"));
static FGameplayTag TAG_State_Fleeing = FGameplayTag::RequestGameplayTag(FName("State.Fleeing"));

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetMesh()->SetupAttachment(GetRootComponent());
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionObjectType(ECC_Mesh);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	
	// Perception (unique names; fine to keep)
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("EnemySightConfig"));
	// ... SightConfig setup ...
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	AbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UHAFAttributeSet>("AttributeSet");

	EnemyHealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthBar"));
	EnemyHealthBar->SetupAttachment(GetRootComponent());

}

void AEnemyBase::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	if (EquippedEnemyRangedWeapon) EquippedEnemyRangedWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
	if (EquippedEnemyRangedWeapon) EquippedEnemyRangedWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	if (EquippedEnemyMeleeWeapon) EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
	if (EquippedEnemyMeleeWeapon) EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	if (EquippedEnemyWeapon) EquippedEnemyWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
	if (EquippedEnemyWeapon) EquippedEnemyWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemyBase::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	if (EquippedEnemyWeapon) EquippedEnemyWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
	if (EquippedEnemyMeleeWeapon) EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
	if (EquippedEnemyRangedWeapon) EquippedEnemyRangedWeapon->GetWeaponMesh()->SetRenderCustomDepth(false);
}

int32 AEnemyBase::GetPlayerLevel()
{
	return Level;
}

void AEnemyBase::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsEnemyDead()) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

	if  (CombatTarget)
	{
		GetTranslationWarpTarget();
		GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocation("TranslationTarget", CombatTarget->GetActorLocation());
		GetRotationWarpTarget();
		GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation("RotationTarget", CombatTarget->GetActorLocation(), CombatTarget->GetActorRotation());
	}
}


void AEnemyBase::TriggerCharm(AActor* InPlayerActor)
{
    if (bIsCharmed || bIsFleeing) return;
    CachedPlayer = InPlayerActor;

    bIsCharmed = true;
    AddStateTag(TAG_State_Charmed);

    // Flip to player's ally team so AISense/attitude treats 'Enemy' team as hostile instead.
    SetGenericTeamId(/*PlayerAlly*/ 1);

    // Nudge BT
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            BB->SetValueAsBool(TEXT("IsCharmed"), true);
            BB->SetValueAsObject(TEXT("PlayerActor"), CachedPlayer);
        }
    }
}

FGenericTeamId AEnemyBase::GetGenericTeamId() const
{
	return TeamId;
}

void AEnemyBase::BeginFlee()
{
    if (bIsFleeing) return;

    bIsCharmed = false;
    RemoveStateTag(TAG_State_Charmed);

    bIsFleeing = true;
    AddStateTag(TAG_State_Fleeing);

    // Optionally reduce friction to help “slide” off ledges
    GetCharacterMovement()->GroundFriction = 0.5f;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            BB->SetValueAsBool(TEXT("IsCharmed"), false);
            BB->SetValueAsBool(TEXT("IsFleeing"), true);
        }
    }

    DoNextFleeHop();
}

void AEnemyBase::DoNextFleeHop()
{
    if (!CachedPlayer) return;
    const FVector Me = GetActorLocation();
    const FVector Player = CachedPlayer->GetActorLocation();
    const FVector AwayDir = (Me - Player).GetSafeNormal();
    const FVector RawDest = Me + AwayDir * FleeHopDistance;

    // Try to project on navmesh to keep moving until a drop is found
    FNavLocation Projected;
    if (const UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        if (Nav->ProjectPointToNavigation(RawDest, Projected))
        {
            if (AAIController* AIC = Cast<AAIController>(GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    BB->SetValueAsVector(TEXT("RunDestination"), Projected.Location);
                }
                AIC->MoveToLocation(Projected.Location, /*AcceptanceRadius=*/50.f);
            }
        }
        else
        {
            // No navmesh further away — just run straight and likely fall
            AddMovementInput(AwayDir, 1.0f);
        }
    }
}

void AEnemyBase::AddStateTag(const FGameplayTag& Tag)
{
    // If you have an ASC, add the loose tag or apply a small GE that grants it.
    // If not using GAS, you can ignore this and rely on bIsCharmed/bIsFleeing.
}

void AEnemyBase::RemoveStateTag(const FGameplayTag& Tag)
{
    // Mirror of AddStateTag
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		EnemiesChaseTarget();
	}
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

void AEnemyBase::Destroyed()
{
	Super::Destroyed();

	if (EquippedEnemyWeapon) EquippedEnemyWeapon->Destroy();
	if (EquippedEnemyMeleeWeapon) EquippedEnemyMeleeWeapon->Destroy();
	else return;
}

void AEnemyBase::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	ClearPatrolTimer();
	ClearAttackTimer();

	static const FName FillainTag("FillainCharacter");
	if (bIsCharmed)
	{
		if (Hitter && Hitter->ActorHasTag(FillainTag))
		{
			return; // refuse to fire/strike
		}
	}
	
	StopMontage(CurrentAttackMontage);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsInsideAttackRadius())
	{
		if (!IsEnemyDead()) StartAttackTimer();
	}
}

void AEnemyBase::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		APawn* Pawn = Cast<APawn>(Actor);
		PawnSeen(Pawn);
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeAbilityActorInfo();

	// --- Enemy Health Bar setup ---
	if (UUserWidget* WidgetObj = EnemyHealthBar->GetUserWidgetObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyHealthBar widget class is: %s"), *WidgetObj->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyHealthBar has no UserWidgetObject!"));
	}
	if (UEnemyProgressBarBaseWidget* HealthBarWidget = Cast<UEnemyProgressBarBaseWidget>(EnemyHealthBar->GetUserWidgetObject()))
	{
		HealthBarWidget->UpdateOwnerEnemy(this);
		HealthBarWidget->SetVisibility(ESlateVisibility::Visible);
		HealthBarWidget->SetWidgetController(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBase::BeginPlay - EnemyHealthBar widget is not of type UEnemyProgressBarBaseWidget"));
	}

	// --- GAS Attribute Delegates ---
	if (const UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HAFAttSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		OnHealthChanged.Broadcast(HAFAttSet->GetHealth());
		OnMaxHealthChanged.Broadcast(HAFAttSet->GetMaxHealth());
	}

	// calls BaseCharacter::BeginPlay
	SafeInitASC_ForPawnOwner();

	// --- Movement setup ---
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = BaseWalkSpeed;
	}

	// --- Tags ---
	Tags.AddUnique(FName("Enemy"));

	// --- AI Perception ---
	if (ensureMsgf(AIPerceptionComponent, TEXT("%s: AIPerceptionComponent is null"), *GetName()))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBase::OnTargetDetected);
	}

	// --- Enemy-specific init ---
	InitializeEnemy();

	// --- Attribute component ---
	if (AttributeComponent)
	{
		DeadEnemySoulCount = AttributeComponent->GetSoulsGathered();
	}
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnemyHealthBar->InitWidget();  // makes sure the UserWidgetObject is created

	if (UEnemyProgressBarBaseWidget* EnemyHealthWidget =
		Cast<UEnemyProgressBarBaseWidget>(EnemyHealthBar->GetUserWidgetObject()))
	{
		EnemyHealthWidget->UpdateOwnerEnemy(this);
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyBase::PossessedBy - ASC is null"));
		return;
	}

	// Use Controller as Owner for AI, Pawn/Character as Avatar.
	AActor* ActorOwner  = NewController ? Cast<AActor>(NewController) : Cast<AActor>(this);
	AActor* AvatarActor = this;

	// Only (re)initialize if the avatar isn't already this pawn.
	if (ASC->GetAvatarActor() != AvatarActor)
	{
		ASC->InitAbilityActorInfo(ActorOwner, AvatarActor);
		ASC->RefreshAbilityActorInfo(); // optional but helps when swapping avatars
		UE_LOG(LogTemp, Verbose, TEXT("EnemyBase::PossessedBy - ASC init: Owner=%s, Avatar=%s"),
			*GetNameSafe(ActorOwner), *GetNameSafe(AvatarActor));
	}

	// Give startup abilities ONCE.
	// Prefer a member bool bStartupAbilitiesGranted = false; on your enemy.
	if (!bStartupAbilitiesGranted)
	{
		UHAFAbilitySystemBlueprintLibrary::GiveStartupAbilities(this, ASC);
		bStartupAbilitiesGranted = true;
	}

	// Apply initial attributes (now that ASC has correct Owner/Avatar).
	if (const UHAFAttributeSet* Attributes = ASC->GetSet<UHAFAttributeSet>())
	{
		Attributes->ApplyInitialValuesForOwner();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBase::PossessedBy - UHAFAttributeSet not found on ASC"));
	}

	if (UEnemyProgressBarBaseWidget* HB = Cast<UEnemyProgressBarBaseWidget>(EnemyHealthBar->GetUserWidgetObject()))
	{
		HB->UpdateOwnerEnemy(this);          // OK
		// Do NOT call HB->OnWidgetConstructed() here.
		// Put setup in UUserWidget::NativeOnInitialized / NativeConstruct.
	}
}

void AEnemyBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// If your ASC lives on the character (AI), InitAbilityActorInfo may already be done.
		if (!ASC->GetAvatarActor()) ASC->InitAbilityActorInfo(this, this);

	}
}

void AEnemyBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) const
{
	const bool bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (bReactingToHit)
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

void AEnemyBase::InitializeAbilityActorInfo()
{
	Super::InitializeAbilityActorInfo();
	if (AbilitySystemComponent) AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();

}

void AEnemyBase::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
	UHAFAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AEnemyBase::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && AttributeComponent)
	{
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
		if (SpawnedSoul) SpawnedSoul->OriginEnemy = this; // or your enemy reference
		int32 EnemySouls = AttributeComponent->GetSoulsGathered();
		SpawnedSoul->SetSoulValue(EnemySouls);
		SpawnedSoul->SetOriginEnemy(this);
	}
}

void AEnemyBase::CharacterDies()
{
	Super::CharacterDies();
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	EnemyHealthBar->GetUserWidgetObject()->RemoveFromParent();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	UE_LOG(LogTemp, Warning, TEXT("Calling SpawnSoul now!"));
	SpawnSoul();
	UE_LOG(LogTemp, Warning, TEXT("SpawnSoul completed!"));
}

int32 AEnemyBase::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);;
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}
void AEnemyBase::MeleeAttack()
{
	Super::MeleeAttack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	if (IsValid(MeleeAttackMontage) && MeleeAttackMontages.IsEmpty()) PlayRandomMajixAttackMontage();
	if (!MeleeAttackMontages.IsEmpty()) PlayRandomMeleeAttackMontage();
}

void AEnemyBase::MajixAttack()
{
	Super::MajixAttack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	if (IsValid(MajixAttackMontage) && MajixAttackMontages.IsEmpty()) PlayRandomMeleeAttackMontage();
	if (!MajixAttackMontages.IsEmpty()) PlayRandomMajixAttackMontage();
	if (!MeleeAttackMontages.IsEmpty() && !MajixAttackMontages.IsEmpty()) PlayAttackMontage(); 
}

void AEnemyBase::PlayAttackMontage()
{
	if (MeleeAttackMontages.Num() >= 1 && MajixAttackMontages.Num() >= 1)
	{
		const int32 RandInt = UKismetMathLibrary::RandomInteger(100);
		if (RandInt >= 1 && RandInt <= 50) PlayRandomMeleeAttackMontage();
		if (RandInt >=51 && RandInt <= 100) PlayRandomMajixAttackMontage();
		return;
	}
}

void AEnemyBase::PlayRandomMeleeAttackMontage()
{
	if (MajixAttackMontages.Num() == 0 && MeleeAttackMontages.Num() >= 1) 
	{
		// Pick a random montage
		int32 Index = FMath::RandRange(0, MeleeAttackMontages.Num() - 1);
		UAnimMontage* SelectedMontage = MeleeAttackMontages[Index];
		if (!SelectedMontage) return;

		// Optional: pick a random section
		FName SectionToPlay = NAME_None;
		const TArray<FCompositeSection>& Sections = SelectedMontage->CompositeSections;
		if (Sections.Num() > 0)
		{
			const int32 SectionIndex = FMath::RandRange(0, Sections.Num() - 1);
			SectionToPlay = Sections[SectionIndex].SectionName;
		}
		// ✅ Store for use in stop/interruption logic
		CurrentAttackMontage = SelectedMontage;
		UE_LOG(LogTemp, Warning, TEXT("🔥 Gnarled is trying to play an attack montage!"));

		// ✅ Play the montage
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		float MontageDuration = AnimInstance->Montage_Play(SelectedMontage);
		if (MontageDuration > 0.f && SectionToPlay != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(SectionToPlay, SelectedMontage);
			UE_LOG(LogTemp, Warning, TEXT("Root motion active: %s"),
			GetCharacterMovement()->IsMovingOnGround() ? TEXT("Grounded") : TEXT("Not grounded"));
		}
	}
}

void AEnemyBase::PlayRandomMajixAttackMontage()
{
	// Pick a random montage
	int32 Index = FMath::RandRange(0, MajixAttackMontages.Num() - 1);
	UAnimMontage* SelectedMontage = MajixAttackMontages[Index];
	if (!SelectedMontage) return;

	// Optional: pick a random section
	FName SectionToPlay = NAME_None;
	const TArray<FCompositeSection>& Sections = SelectedMontage->CompositeSections;
	if (Sections.Num() > 0)
	{
		const int32 SectionIndex = FMath::RandRange(0, Sections.Num() - 1);
		SectionToPlay = Sections[SectionIndex].SectionName;
	}
	// ✅ Store for use in stop/interruption logic
	CurrentAttackMontage = SelectedMontage;
	UE_LOG(LogTemp, Warning, TEXT("🔥 Enemy is playing an attack montage!"));

	// ✅ Play the montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	float MontageDuration = AnimInstance->Montage_Play(SelectedMontage);
	if (MontageDuration > 0.f && SectionToPlay != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(SectionToPlay, SelectedMontage);
		UE_LOG(LogTemp, Warning, TEXT("Root motion active: %s"),
		GetCharacterMovement()->IsMovingOnGround() ? TEXT("Grounded") : TEXT("Not grounded"));
	}
}

void AEnemyBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == CurrentAttackMontage)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		CurrentAttackMontage = nullptr;
	}
}

bool AEnemyBase::CanAttack()
{
	bool bCanAttack = IsInsideAttackRadius() &&
		!IsEnemyAttacking() &&
		!IsEnemyEngaged() &&
		!IsEnemyDead();
	return bCanAttack;
}


void AEnemyBase::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}



void AEnemyBase::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::HandleDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
}

void AEnemyBase::InitializeAbilitySystem()
{
	if (AbilitySystemComponent) AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AEnemyBase::InitializeEnemy()
{
	LaunchEnemyAIController();
	MoveToTarget(PatrolTarget);
	EnemyHealthBar->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	SpawnEnemyWeapon();
	InitializeAbilitySystem();
}


AAIController* AEnemyBase::LaunchEnemyAIController()
{
	EnemyController = Cast<AAIController>(GetController());
	return EnemyController;;
}

void AEnemyBase::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBase::PatrolTimerFinished, WaitTime);
	}
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

void AEnemyBase::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemyBase::EnemiesLoseInterest()
{
	CombatTarget = nullptr;
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

bool AEnemyBase::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemyBase::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemyBase::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemyBase::IsEnemyChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemyBase::IsEnemyAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemyBase::IsEnemyEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemyBase::IsEnemyDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

void AEnemyBase::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemyBase::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyBase::MeleeAttack, AttackTime);
}

void AEnemyBase::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemyBase::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemyBase::MoveToTarget(AActor* Target)
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

void AEnemyBase::SpawnEnemyWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeaponBase* DefaultWeapon = World->SpawnActor<AWeaponBase>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedEnemyWeapon = DefaultWeapon;
		EquippedWeapon = DefaultWeapon; // ✅ also assign to BaseCharacter's pointer
		EquippedEnemyMeleeWeapon = Cast<AMeleeWeapon>(EquippedEnemyWeapon);
		EquippedMeleeWeapon = EquippedEnemyMeleeWeapon;
		if (EquippedEnemyMeleeWeapon->WeaponBox)
		{
			EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
			EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EquippedEnemyMeleeWeapon->WeaponBox->SetGenerateOverlapEvents(true);
			EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
			EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
			EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
		}
		EquippedEnemyMeleeWeapon->bIsEquipped = true;
		EquippedWeapon->bIsEquipped = true;
		EquippedMeleeWeapon->bIsEquipped = true;
	}
}


void AEnemyBase::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		!IsEnemyDead() && 
		!IsEnemyChasing() &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		EnemiesChaseTarget();
	}
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (HealthChangedHandle.IsValid())
		{
			const UHAFAttributeSet* HAFSet = ASC->GetSet<UHAFAttributeSet>();
			if (HAFSet)
			{
				ASC->GetGameplayAttributeValueChangeDelegate(HAFSet->GetHealthAttribute())
				   .Remove(HealthChangedHandle);
			}
		}
		if (MaxHealthChangedHandle.IsValid())
		{
			const UHAFAttributeSet* HAFSet = ASC->GetSet<UHAFAttributeSet>();
			if (HAFSet)
			{
				ASC->GetGameplayAttributeValueChangeDelegate(HAFSet->GetMaxHealthAttribute())
				   .Remove(MaxHealthChangedHandle);
			}
		}
		if (HitReactChangedHandle.IsValid())
		{
			ASC->RegisterGameplayTagEvent(FHAFGameplayTags::Get().Effects_HitReact,
										  EGameplayTagEventType::NewOrRemoved)
			   .Remove(HitReactChangedHandle);
		}
	}

	Super::EndPlay(EndPlayReason);
}