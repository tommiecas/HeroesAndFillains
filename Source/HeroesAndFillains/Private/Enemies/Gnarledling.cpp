// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Gnarledling.h"

#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"

AGnarledling::AGnarledling()
{
	EnemyDisplayName = "a baby Gnarledling!";

	
}

void AGnarledling::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGnarledling::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(RightFistCollision, TEXT("CollisionComp is null")))
	{
		RightFistCollision->OnComponentBeginOverlap.AddDynamic(
			this, &AGnarledling::HandleFistBeginOverlap
		);
	}

	if (ensureMsgf(LeftFistCollision, TEXT("CollisionComp is null")))
	{
		LeftFistCollision->OnComponentBeginOverlap.AddDynamic(
			this, &AGnarledling::HandleFistBeginOverlap
		);
	}
}

// The dynamic-delegate entry point (has UFUNCTION)
void AGnarledling::HandleFistBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Forward to your real logic (the override), or directly keep the logic here.
	OnFistOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

// Your existing logic (override; NO UFUNCTION here)
void AGnarledling::OnFistOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !RightFistDamagedActors.Contains(Player) && !LeftFistDamagedActors.Contains(Player))
	{
		RightFistDamagedActors.Add(Player);
		LeftFistDamagedActors.Add(Player);

		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, LittleFistDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false;
	}

	GetWorld()->GetTimerManager().SetTimer(
		FistDamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.25f, false
	);
}

void AGnarledling::EnableLeftFist()
{
	Super::EnableLeftFist();
}

void AGnarledling::DisableLeftFist()
{
	Super::DisableLeftFist();
}

void AGnarledling::EnableRightFist()
{
	Super::EnableRightFist();
}

void AGnarledling::DisableRightFist()
{
	Super::DisableRightFist();
}

void AGnarledling::ResetCanDamage()
{
	Super::ResetCanDamage();
}

int32 AGnarledling::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	return Selection;
}

