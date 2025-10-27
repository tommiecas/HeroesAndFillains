// EnemyCombatBase.cpp
#include "Enemies/CombatCollisionsAmendedEnemyBaseClass.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACombatCollisionsAmendedEnemyBaseClass::ACombatCollisionsAmendedEnemyBaseClass()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACombatCollisionsAmendedEnemyBaseClass::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatCollisionsAmendedEnemyBaseClass::RegisterAttackCollision(UBoxComponent* CollisionBox)
{
	if (!CollisionBox) return;

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionObjectType(ECC_Pawn);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACombatCollisionsAmendedEnemyBaseClass::OnAttackCollisionOverlap);

	AttackCollisions.AddUnique(CollisionBox);
}

void ACombatCollisionsAmendedEnemyBaseClass::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
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
}

void ACombatCollisionsAmendedEnemyBaseClass::OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanDamage || !OtherActor) return;

	if (DamagedActors.Contains(OtherActor)) return;
	DamagedActors.Add(OtherActor);

	UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetController(), this, nullptr);

	bCanDamage = false;
	GetWorldTimerManager().SetTimer(DamageResetTimer, this, &ACombatCollisionsAmendedEnemyBaseClass::ResetCanDamage, 0.25f, false);
}

void ACombatCollisionsAmendedEnemyBaseClass::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bCanDamage = true;
	DamagedActors.Empty();
	SetEnemyState(EEnemyState::EES_Idle);
}

void ACombatCollisionsAmendedEnemyBaseClass::MeleeAttack()
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
				EndDelegate.BindUObject(this, &ACombatCollisionsAmendedEnemyBaseClass::OnAttackMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate);
			}
		}
	}
}

void ACombatCollisionsAmendedEnemyBaseClass::ResetCanDamage()
{
	bCanDamage = true;
	DamagedActors.Empty();
}
