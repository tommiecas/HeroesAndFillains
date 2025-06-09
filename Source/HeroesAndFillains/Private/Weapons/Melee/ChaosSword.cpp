// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/ChaosSword.h"
#include "Characters/FillainCharacter.h"


AChaosSword::AChaosSword()
	: AMeleeWeapon()
{
	InitialMeshScale = FVector(0.01f);
}

void AChaosSword::BeginPlay()
{
	Super::BeginPlay();
    
	if (WeaponMesh)
	{
		WeaponMesh->SetRelativeScale3D(FVector(0.01f));
	}
}

void AChaosSword::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AChaosSword::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

void AChaosSword::OnEquippedTwoHanded()
{
	UE_LOG(LogTemp, Warning, TEXT("ChaosSword OnEquippedTwoHanded - Before: Scale = %s"), 
		  *WeaponMesh->GetRelativeScale3D().ToString());
    
	if (WeaponMesh)
	{
		WeaponMesh->SetRelativeScale3D(FVector(0.01f));
	}
        
	Super::OnEquippedTwoHanded();
        
	if (WeaponMesh)
	{
		WeaponMesh->SetRelativeScale3D(FVector(0.01f));
		UE_LOG(LogTemp, Warning, TEXT("ChaosSword OnEquippedTwoHanded - After: Scale = %s"), 
			   *WeaponMesh->GetRelativeScale3D().ToString());
	}

}

void AChaosSword::OnRep_WeaponState()
{
	Super::OnRep_WeaponState();

	// Maintain scale when weapon state changes
	if (WeaponMesh)
	{
		WeaponMesh->SetRelativeScale3D(InitialMeshScale);
	}

}

void AChaosSword::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	// Set scale before attachment
	if (WeaponMesh)
	{
		WeaponMesh->SetWorldScale3D(InitialMeshScale);
	}
    
	Super::AttachMeshToSocket(InParent, InSocketName);
    
	// Ensure scale after attachment
	if (WeaponMesh)
	{
		WeaponMesh->SetWorldScale3D(InitialMeshScale);
	}


}

