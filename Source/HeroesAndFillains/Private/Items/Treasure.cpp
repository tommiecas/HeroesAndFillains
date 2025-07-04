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

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ATreasure::OnSphereOverlap);

}
void ATreasure::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Treasure overlapped by %s"), *FillainCharacter->GetName());

		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		Destroy(); // 💥 Check if this is being reached
	}
}

void ATreasure::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
