// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"

#include "Characters/FillainCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Components/SceneComponent.h"
#include "Pickups/AmmoPickUp.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

AWeaponBase::AWeaponBase()
	: Super() 
{

}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponBase, WeaponState);
	DOREPLIFETIME(AWeaponBase, WeaponCategory);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	
}



void AWeaponBase::OnRep_WeaponCategory()
{

}

void AWeaponBase::OnRep_WeaponState()
{
	
}

void AWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr)
	{
		FillainOwnerCharacter = nullptr;
		FillainOwnerController = nullptr;
	}
}


void AWeaponBase::SetHandsNeeded(AWeaponBase* WeaponBase)
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		HandsNeeded = EHandsNeeded::EHN_TwoHandedWeapon;
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword || WeaponType == EWeaponType:: EWT_SandSword || WeaponType == EWeaponType:: EWT_SoulSword || WeaponType == EWeaponType:: EWT_ShadowSword || WeaponType == EWeaponType:: EWT_SkyMace || WeaponType ==EWeaponType::EWT_MajixProjectile || WeaponType == EWeaponType::EWT_FireBolt)
	{
		HandsNeeded = EHandsNeeded::EHN_OneHandedWeapon;
	}
}

void AWeaponBase::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}



void AWeaponBase::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}



void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex);
}




void AWeaponBase::SetOneOrTwoHandedWeapon(AWeaponBase* Weapon)
{
	if (!Weapon) return;
    
	// Validate the weapon's current state
	if (Weapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded ||
		Weapon->WeaponState == EWeaponState::EWS_EquippedOneHanded)
	{
		// Handle already equipped weapon
		return;
	}

	if (Weapon->WeaponType == EWeaponType::EWT_RocketLauncher || 
		Weapon->WeaponType == EWeaponType::EWT_GrenadeLauncher || 
		Weapon->WeaponType == EWeaponType::EWT_SniperRifle || 
		Weapon->WeaponType == EWeaponType::EWT_Shotgun || 
		Weapon->WeaponType == EWeaponType::EWT_ChaosSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		TwoHandedWeapon = Weapon;  // Store the weapon reference correctly
	}
	else if (Weapon->WeaponType == EWeaponType::EWT_AssaultRifle || 
			 Weapon->WeaponType == EWeaponType::EWT_SubmachineGun || 
			 Weapon->WeaponType == EWeaponType::EWT_Pistol || 
			 Weapon->WeaponType == EWeaponType::EWT_RubySword || 
			 Weapon->WeaponType == EWeaponType::EWT_SapphireSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
		OneHandedWeapon = Weapon;  // Store the weapon reference correctly
	}
}

void AWeaponBase::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
}

