// Fill out your copyright notice in the Description page of Project Settings.



#include "Enemies/Gnarled.h"

AGnarled::AGnarled()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGnarled::BeginPlay()
{
	Super::BeginPlay();
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

