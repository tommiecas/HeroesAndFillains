// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"

#include <Items/Soul.h>

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeroesAndFillains/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAFComponents/AttributeComponent.h"
#include "HUD/EnemyHealthBarWidget.h"
#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Weapons/WeaponBase.h"
#include "HAFComponents/AttributeComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Weapons/Ranged/RangedWeapon.h"


AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECollisionResponse::ECR_Ignore);
	
	// Create the WidgetComponent
	HealthBarWidgetComponent = CreateDefaultSubobject<UEnemyHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(300.f, 25.f));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1600.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	
	DeadEnemySoulCount = AttributeComponent->GetSoulsGathered();

	AbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UHAFAttributeSet>(TEXT("AttributeSet"));
}

void AEnemyBase::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	if (EquippedEnemyRangedWeapon)
	{
		EquippedEnemyRangedWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
		EquippedEnemyRangedWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
	if (EquippedEnemyMeleeWeapon)
	{
		EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
		EquippedEnemyMeleeWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
	if (EquippedEnemyWeapon)
	{
		EquippedEnemyWeapon->GetWeaponMesh()->SetRenderCustomDepth(true);
		EquippedEnemyWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
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
	if (!IsEnemyDead()) ShowHealthBarWidgetComponent();
	ClearPatrolTimer();
	ClearAttackTimer();
	
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
		UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *Actor->GetName());
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBase::OnTargetDetected);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));

	InitializeAbilityActorInfo();
}

void AEnemyBase::InitializeAbilityActorInfo()
{
	if (AbilitySystemComponent) AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

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
	HideHealthBarWidgetComponent();
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
	if (IsValid(MeleeAttackMontage) && MeleeAttackMontages.IsEmpty()) PlayMeleeAttackMontage();
	if (!MeleeAttackMontages.IsEmpty()) PlayRandomAttackMontage();
}

int32 AEnemyBase::PlayMeleeAttackMontage()
{
	const int32 Selection = Super::PlayMeleeAttackMontage();
	return Selection;
}

void AEnemyBase::PlayRandomAttackMontage()
{
	if (MeleeAttackMontages.Num() == 0 || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

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

	// ✅ Disable movement so root motion can fully take over
	UCharacterMovementComponent* CharMove = GetCharacterMovement();
	if (CharMove && SelectedMontage->GetName().StartsWith("Root_"))
	{
		CharMove->DisableMovement();
		CharMove->StopMovementImmediately();
	}

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

	// ✅ Optional: Set up callback to re-enable movement
	AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyBase::OnAttackMontageEnded);
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
	HideHealthBarWidgetComponent();
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

void AEnemyBase::HideHealthBarWidgetComponent()
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
}

void AEnemyBase::ShowHealthBarWidgetComponent()
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility((true));
	}
}

void AEnemyBase::EnemiesLoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBarWidgetComponent();
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