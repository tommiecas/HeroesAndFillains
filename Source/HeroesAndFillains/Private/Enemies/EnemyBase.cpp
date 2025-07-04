// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
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
#include "HUD/HealthBarWidget.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "Components/BoxComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Weapons/WeaponBase.h"
#include "HAFComponents/AttributeComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Melee/MeleeWeapon.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);

	// Create the WidgetComponent
	NewHealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	NewHealthBarWidgetComponent->SetupAttachment(GetCapsuleComponent());
	NewHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NewHealthBarWidgetComponent->SetDrawSize(FVector2D(300.f, 25.f));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	EnemyCombat = CreateDefaultSubobject<UCombatComponent>(TEXT("EnemyCombat"));
	EnemyCombat->SetIsReplicated(true);
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
		EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
		EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EquippedEnemyMeleeWeapon->WeaponBox->SetGenerateOverlapEvents(true);
		EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
		
	}
}

void AEnemyBase::HideHealthBarWidget()
{
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility(false);
	}
}

void AEnemyBase::LaunchEnemyAIController()
{
	EnemyController = Cast<AAIController>(GetController());
}

void AEnemyBase::InitializeEnemy()
{
	HideHealthBarWidget();
	LaunchEnemyAIController();
	MoveToTarget(PatrolTarget);
	SpawnEnemyWeapon();
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyCombat)
	{
		EnemyCombat->SetEnemy(this); // this is AEnemyBase*
	}

	EnemyCombat = Cast<UCombatComponent>(GetComponentByClass(UCombatComponent::StaticClass()));
    
	if (!EnemyCombat)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ EnemyCombat is null in BeginPlay!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ EnemyCombat successfully assigned in BeginPlay"));
	}
	
	InitializeEnemy();
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyBase::PawnSeen);
	Tags.Add(FName("Enemy"));

	if (!EnemyCombat)
	{
		EnemyCombat = FindComponentByClass<UCombatComponent>();
		if (!EnemyCombat)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ EnemyCombat is NULL on %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("✅ EnemyCombat found at runtime: %s"), *GetName());
		}
	}
	

}

void AEnemyBase::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemyBase::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	Super::SetWeaponCollisionEnabled(CollisionEnabled);
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
	MoveRequest.SetAcceptanceRadius(60.f);
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

void AEnemyBase::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
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



void AEnemyBase::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
	
}


void AEnemyBase::HideHealthBar()
{
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
}

void AEnemyBase::ShowHealthBar()
{
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((true));
	}
}

void AEnemyBase::EnemiesLoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemyBase::EnemiesStartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

bool AEnemyBase::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

void AEnemyBase::EnemiesChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemyBase::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemyBase::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemyBase::IsEnemyDead()
{
	return EnemyState == EEnemyState::EES_Dead;
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

void AEnemyBase::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBase::PatrolTimerFinished, WaitTime);
	}
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
	UE_LOG(LogTemp, Warning, TEXT("💥 AEnemyBase::TakeDamage — DamageAmount: %f"), DamageAmount);
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CalculateDamage(this, DamageAmount, EventInstigator);
	CombatTarget = EventInstigator->GetPawn();
	EnemiesChaseTarget();
	return DamageAmount;
}

void AEnemyBase::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
	ShowHealthBar();
}

void AEnemyBase::CharacterDies()
{
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AEnemyBase::Destroyed()
{
	Super::Destroyed();

	if (EquippedEnemyWeapon) EquippedEnemyWeapon->Destroy();
	if (EquippedEnemyMeleeWeapon) EquippedEnemyMeleeWeapon->Destroy();
	else return;
}

float AEnemyBase::CalculateDamage(AActor* DamagedPawn, float DamageAmount, AController* InstigatorController)
{
	Super::CalculateDamage(DamagedPawn, DamageAmount, InstigatorController);
	return DamageAmount;
}

int32 AEnemyBase::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void AEnemyBase::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("🔍 Enemy %s attacking — EnemyCombat is %s"),
	*GetName(),
	EnemyCombat ? TEXT("VALID") : TEXT("NULL"));
	EnemyState = EEnemyState::EES_Engaged;
	Super::MeleeAttack();
	PlayMeleeAttackMontage();
}

int32 AEnemyBase::PlayMeleeAttackMontage()
{
	const int32 Selection = Super::PlayMeleeAttackMontage();
	return Selection;
}

bool AEnemyBase::CanAttack()
{
	bool bCanAttack = IsInsideAttackRadius() &&
		!IsEnemyAttacking() &&
		!IsEnemyEngaged() &&
		!IsEnemyDead();
	return bCanAttack;
}

void AEnemyBase::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	Super::PlayMontageSection(Montage, SectionName);
}



