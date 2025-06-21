// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StormAssassin.h"

#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarWidgetComponent.h"

AStormAssassin::AStormAssassin()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AStormAssassin::BeginPlay()
{
	Super::BeginPlay();

	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
}

void AStormAssassin::PlayHitReactMontage(const FName& SectionName)
{
	Super::PlayHitReactMontage(SectionName);
}

void AStormAssassin::PlayDeathMontage()
{
	Super::PlayDeathMontage();
	
	if (!IsValid(DeathMontage))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); 
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		int32 Selection = FMath::RandRange(0, 3);               
		FName Section;

		switch (Selection)                                      
		{                                                       
		case 0: Section = FName("Death1"); StormDeath = EStormDeath::ESD_Death1; break;
		case 1: Section = FName("Death2"); StormDeath = EStormDeath::ESD_Death2; break;
		}                                                       

		UE_LOG(LogTemp, Warning, TEXT("🎯 Playing Section: %s"), *Section.ToString());
		AnimInstance->Montage_JumpToSection(Section, DeathMontage);
		if (DeathMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("✅ DeathMontage assigned: %s"), *DeathMontage->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ DeathMontage is NULL"));
		}
		if (!DeathMontage->IsValidSectionName(Section))
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Invalid section name: %s"), *Section.ToString());
		}

		if (DeathMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trying to play montage: %s"), *DeathMontage->GetName());
			AnimInstance->Montage_Stop(0.1f);
			float Result = AnimInstance->Montage_Play(DeathMontage, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("Montage_Play returned: %f"), Result);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DeathMontage is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No AnimInstance on enemy mesh!"));
	}
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

float AStormAssassin::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AStormAssassin::EnemyDies()
{
	PlayDeathMontage();
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}
