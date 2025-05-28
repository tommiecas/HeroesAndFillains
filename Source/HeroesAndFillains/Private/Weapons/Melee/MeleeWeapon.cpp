// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/WeaponBase.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponTypes.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{

}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMeleeWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
}

void AMeleeWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
}

void AMeleeWeapon::OnEquippedOneHanded()
{
	Super::OnEquippedOneHanded();
}

void AMeleeWeapon::OnEquippedTwoHanded()
{
	Super::OnEquippedTwoHanded();
}


void AMeleeWeapon::OnDropped()
{
	Super::OnDropped();
	
}

void AMeleeWeapon::OnEquippedSecondary()
{
	Super::OnEquippedSecondary();
}
	
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
}

void AMeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMeleeWeapon::SetEquippedMeleeWeaponState()
{
	if (MeleeWeaponType == EMeleeType::EMT_RubySword || MeleeWeaponType == EMeleeType::EMT_SapphireSword)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
	if (MeleeWeaponType == EMeleeType::EMT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
}