// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StormAssassin.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Melee/StormWeapons.h"

AStormAssassin::AStormAssassin()
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
}

void AStormAssassin::BeginPlay()
{
	Super::BeginPlay();

	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
}


void AStormAssassin::AttackEnd()
{
	Super::AttackEnd();
}

void AStormAssassin::PlayHitReactMontage(const FName& SectionName)
{
	Super::PlayHitReactMontage(SectionName);
}

int32 AStormAssassin::PlayDeathMontage()
{
	Super::PlayDeathMontage();
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

int32 AStormAssassin::PlayMeleeAttackMontage()
{
	Super::PlayMeleeAttackMontage();
	return PlayRandomMontageSection(MeleeAttackMontage, MeleeAttackMontageSections);
}

void AStormAssassin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStormAssassin::GetHit_Implementation(const FVector& ImpactPoint)
{
	// Always call parent implementation first
	Super::GetHit_Implementation(ImpactPoint);
	

	// Add any Gnarled-specific hit reaction logic here
	// Make sure to add null checks for any component or asset references
	if (!IsValid(this))
	{
		return;
	}

	// Example of safe hit reaction logic
	DirectionalHitReact(ImpactPoint);
}

void AStormAssassin::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedEnemyMeleeWeapon->WeaponBox)
	{
		EquippedEnemyMeleeWeapon->WeaponBox->SetCollisionEnabled(CollisionEnabled);
		EquippedEnemyMeleeWeapon->WeaponBox->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);

		UE_LOG(LogTemp, Warning, TEXT("WeaponBox collision enabled: %d | OverlapEvents: %d"),
				   CollisionEnabled == ECollisionEnabled::QueryOnly,
				   EquippedEnemyMeleeWeapon->WeaponBox->GetGenerateOverlapEvents());
	}

	if (CollisionEnabled == ECollisionEnabled::NoCollision)
	{
		AStormWeapons* StormWeapon = Cast<AStormWeapons>(EquippedEnemyMeleeWeapon);
		StormWeapon->DamagedActors.Empty();
		UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared at end of swing"));
	}
}


float AStormAssassin::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AStormAssassin::CharacterDies()
{
	Super::CharacterDies();
}

void AStormAssassin::MeleeAttack()
{
	Super::MeleeAttack();
}
