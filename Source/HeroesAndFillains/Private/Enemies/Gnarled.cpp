// Fill out your copyright notice in the Description page of Project Settings.



#include "Enemies/Gnarled.h"

#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarWidgetComponent.h"

AGnarled::AGnarled()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent->SetWorldScale3D(FVector(3.0f));

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

void AGnarled::PlayDeathMontage()
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
		case 0: Section = FName("Death1"); GnarledDeath = EGnarledDeath::EGD_Death1; break;
		case 1: Section = FName("Death2"); GnarledDeath = EGnarledDeath::EGD_Death2; break;
		case 2: Section = FName("Death3"); GnarledDeath = EGnarledDeath::EGD_Death3; break;
		case 3: Section = FName("Death4"); GnarledDeath = EGnarledDeath::EGD_Death4; break;
		default: Section = FName("Death1"); GnarledDeath = EGnarledDeath::EGD_Death1; break;
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

void AGnarled::EnemyDies()
{
	PlayDeathMontage();
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}

