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

	RightFistCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnarledling::OnFistOverlap);
	LeftFistCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnarledling::OnFistOverlap);
}

void AGnarledling::OnFistOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		FistDamageResetTimer,
		this,
		&AGnarledling::ResetCanDamage,
		0.25f, // or however long the punch takes
		false
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

