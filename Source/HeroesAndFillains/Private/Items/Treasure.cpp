// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"

#include "Characters/FillainCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
	: Super()
{
	
	
}

void ATreasure::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentBeginOverlap.RemoveDynamic(this, &ATreasure::OnSphereOverlap);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ATreasure::OnSphereOverlap);
}
void ATreasure::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddGoldAcquiredToTotalGold(this);
		SpawnPickupSound();
		Destroy(); // 💥 Check if this is being reached
	}
}

void ATreasure::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
