// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakables/BreakableActorBase.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

ABreakableActorBase::ABreakableActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); 
	Capsule->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap); 

}

void ABreakableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActorBase::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (bBroken) return;
	bBroken = true;
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
}

