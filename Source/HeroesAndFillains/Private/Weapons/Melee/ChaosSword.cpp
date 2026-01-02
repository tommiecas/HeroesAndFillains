// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/ChaosSword.h"
#include "Characters/FillainCharacter.h"


AChaosSword::AChaosSword()
	: AMeleeWeapon()
{

}



void AChaosSword::BeginAttack()
{
	Super::BeginAttack();
}

void AChaosSword::TickAttackTrace()
{
	Super::TickAttackTrace();
}

void AChaosSword::TraceBetweenPoints(FVector& LastLocation, USceneComponent* TracePoint)
{
	Super::TraceBetweenPoints(LastLocation, TracePoint);
}

void AChaosSword::EndAttack()
{
	Super::EndAttack();
}

void AChaosSword::BeginPlay()
{
	Super::BeginPlay();
	
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
	// UE_LOG(LogTemp, Warning, TEXT("ChaosSword OnEquippedTwoHanded - Before: Scale = %s"), 
		//  *WeaponMesh->GetRelativeScale3D().ToString());
    
	
        
	Super::OnEquippedTwoHanded();
        
	

}

void AChaosSword::OnRep_WeaponState()
{
	Super::OnRep_WeaponState();

	// Maintain scale when weapon state changes
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetRelativeScale3D(InitialMeshScale);
	}

}

void AChaosSword::Equip(USceneComponent* InParent, FName InSocketName,  AActor* NewOwner, APawn* NewInstigator)
{
	// Set scale before attachment
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetWorldScale3D(InitialMeshScale);
	}
    
	Super::Equip(InParent, InSocketName, NewOwner, NewInstigator);
    
	// Ensure scale after attachment
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetWorldScale3D(InitialMeshScale);
	}


}

