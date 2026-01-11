// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/HAFCheckpoint.h"

#include "Components/SphereComponent.h"
#include "GameMode/HAFHybridGameMode.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AHAFCheckpoint::AHAFCheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);


	
}

void AHAFCheckpoint::LoadActor_Implementation()
{
	if (bReached) HandleGlowEffects();
}

void AHAFCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AHAFCheckpoint::OnSphereOverlap);
}

void AHAFCheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		if (AHAFHybridGameMode* HAFGM = Cast<AHAFHybridGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			HAFGM->SaveWorldState(GetWorld());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		
		HandleGlowEffects();
	}
}

void AHAFCheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}
