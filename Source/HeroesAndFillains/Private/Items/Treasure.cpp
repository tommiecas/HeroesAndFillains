// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"

#include "Characters/FillainCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	// DO NOT re-create Root. It's already created in AItem
	check(Root); // Safe to use since AItem sets it as RootComponent

	// Create and attach TreasureMesh
	TreasureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreasureMesh"));
	TreasureMesh->SetupAttachment(Root); // ✅ Use inherited Root

	// Configure inherited AreaSphere (created in AItem)
	check(AreaSphere); // Ensure it was initialized in AItem

	AreaSphere->SetSphereRadius(200.f);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	AreaSphere->SetCollisionProfileName("OverlapAllDynamic");
	AreaSphere->SetCollisionObjectType(ECC_Pawn);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ATreasure::OnSphereOverlap);
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
