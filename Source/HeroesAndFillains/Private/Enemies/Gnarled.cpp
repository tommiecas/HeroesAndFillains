// Fill out your copyright notice in the Description page of Project Settings.



#include "Enemies/Gnarled.h"

#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "Animation/AnimInstance.h"

AGnarled::AGnarled()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGnarled::BeginPlay()
{
	Super::BeginPlay();

	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
}

void AGnarled::PlayHitReactMontage(const FName& SectionName)
{
	Super::PlayHitReactMontage(SectionName);
}

int32 AGnarled::PlayDeathMontage()
{
	Super::PlayDeathMontage();
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

int32 AGnarled::PlayMeleeAttackMontage()
{
	Super::PlayMeleeAttackMontage();
	return PlayRandomMontageSection(MeleeAttackMontage, MeleeAttackMontageSections);
}

void AGnarled::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGnarled::GetHit_Implementation(const FVector& ImpactPoint)
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

float AGnarled::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AGnarled::CharacterDies()
{
	Super::CharacterDies();
}

void AGnarled::MeleeAttack()
{
	Super::MeleeAttack();
}

