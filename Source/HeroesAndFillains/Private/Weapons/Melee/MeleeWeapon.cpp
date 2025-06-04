// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/WeaponBase.h"
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

void AMeleeWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;
}

void AMeleeWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
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
	if (MeleeType == EMeleeType::EMT_RubySword || MeleeType == EMeleeType::EMT_SapphireSword)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
	if (MeleeType == EMeleeType::EMT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
}

void AMeleeWeapon::SetMeleeWeaponInformationText(UWidgetComponent* MeleeWidgetComponent, AMeleeWeapon* MeleeWeapon)
{
		UItemInfoWidgetBase* MeleeWidget = Cast<UItemInfoWidgetBase>(MeleeWidgetComponent->GetUserWidgetObject());
		if (MeleeWeapon && MeleeWidget)
		{
			if (MeleeWidget->Line1) MeleeWidget->Line1->SetText(FText::FromString(MeleeWeapon->MeleeWeaponName));
			if (MeleeWidget->Line2) MeleeWidget->Line2->SetText(FText::FromString(MeleeWeapon->MeleeWeaponHistory));
			if (MeleeWidget->Line3) MeleeWidget->Line3->SetText(FText::FromString(MeleeWeapon->MeleeWeaponResistances));
			if (MeleeWidget->Line4) MeleeWidget->Line4->SetText(FText::FromString(MeleeWeapon->MeleeWeaponWeaknesses));
			if (MeleeWidget->Line5) MeleeWidget->Line5->SetText(FText::FromString(MeleeWeapon->MeleeWeaponDamage));
	}
}