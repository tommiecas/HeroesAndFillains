// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ItemInfoWidgetBase.h"
#include "Components/TextBlock.h"

#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Pickups/AmmoPickup.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Components/WidgetComponent.h"


void UItemInfoWidgetBase::SetMeleeInfo(const FString& Name, const FString& History,
									 const FString& Resistances, const FString& Weaknesses,
									 const FString& Damage)
{
	UWidgetComponent* WidgetComponentA = this->OwningWidgetComponent;
	if (WidgetComponentA)
	{
		AActor* OwnerActor = WidgetComponentA->GetOwner();
		if (AMeleeWeapon* InfolessMeleeWeapon = Cast<AMeleeWeapon>(OwnerActor))
		{
			InfolessMeleeWeapon->MeleeWeaponName = Name;
			InfolessMeleeWeapon->MeleeWeaponHistory = History;
			InfolessMeleeWeapon->MeleeWeaponResistances = Resistances;
			InfolessMeleeWeapon->MeleeWeaponWeaknesses = Weaknesses;
			InfolessMeleeWeapon->MeleeWeaponDamage = Damage;
			
			if (Line1) Line1->SetText(FText::FromString(Name));
			if (Line2) Line2->SetText(FText::FromString(History));
			if (Line3) Line3->SetText(FText::FromString(Resistances));
			if (Line4) Line4->SetText(FText::FromString(Weaknesses));
			if (Line5) Line5->SetText(FText::FromString(Damage));
		}
	}
}

void UItemInfoWidgetBase::SetRangedInfo(const FString& Name, const FString& Description,
									  const FString& Type, const FString& Rarity,
									  const FString& Damage)
{
	UWidgetComponent* WidgetComponentA = this->OwningWidgetComponent;
	if (WidgetComponentA)
	{
		AActor* OwnerActor = WidgetComponentA->GetOwner();
		if (ARangedWeapon* InfolessRangedWeapon = Cast<ARangedWeapon>(OwnerActor))
		{
			if (Line1) Line1->SetText(FText::FromString(Name));
			if (Line2) Line2->SetText(FText::FromString(Description));
			if (Line3) Line3->SetText(FText::FromString(Type));
			if (Line4) Line4->SetText(FText::FromString(Rarity));
			if (Line5) Line5->SetText(FText::FromString(Damage));
			UE_LOG(LogTemp, Warning, TEXT("SetRangedInfo(): Name=%s, Desc=%s"), *Name, *Description);
		}
	}
}


void UItemInfoWidgetBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* if (UWidgetComponent* Component = Cast<UWidgetComponent>(this))
	{
		if (AActor* GearPickup = Cast<AActor>(Component->GetOwner()))
		{
			if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(GearPickup))
			{
				Ranged->ShowPickupAndInfoWidgets(true);
			}
			else if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(GearPickup))
			{
				Melee->ShowPickupAndInfoWidgets(true);
			}
			else if (AAmmoPickup* Ammo = Cast<AAmmoPickup>(GearPickup))
			{
				Ammo->ShowPickupAndInfoWidgets(true);
			}
		}
	} */
}

void UItemInfoWidgetBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* if (UWidgetComponent* Component = Cast<UWidgetComponent>(this))
	{
		if (AActor* GearPickup = Cast<AActor>(Component->GetOwner()))
		{
			if (ARangedWeapon* Ranged = Cast<ARangedWeapon>(GearPickup))
			{
				Ranged->ShowPickupAndInfoWidgets(false);
			}
			else if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(GearPickup))
			{
				Melee->ShowPickupAndInfoWidgets(false);
			}
			else if (AAmmoPickup* Ammo = Cast<AAmmoPickup>(GearPickup))
			{
				Ammo->ShowPickupAndInfoWidgets(false);
			}
		}
	} */
}
