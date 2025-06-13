// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"
#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/AmmoPickupIntelWidget.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupGearWidget.h"

AAmmoPickup::AAmmoPickup()
	: Super()
{
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		UCombatComponent* Combat = FillainCharacter->GetCombatComponent();
		if (Combat)
		{
			Combat->PickupAmmo(RangedType, AmountOfAmmoInside);
		}
	}
	Destroy();
}

void AAmmoPickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	// Fade in Info Widget
	if (ItemInfoWidgetComponent)
	{
		UItemInfoWidgetBase* InfoInstancedWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
		if (InfoInstancedWidgetA && InfoInstancedWidgetA->FadeOutAnimation)
		{
			InfoInstancedWidgetA->PlayAnimation(InfoInstancedWidgetA->FadeOutAnimation);
		}
	}

	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponent)
	{
		UPickupGearWidget* PickupGearInstancedWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupGearInstancedWidgetA && PickupGearInstancedWidgetA->FadeOutAnimation)
		{
			PickupGearInstancedWidgetA->PlayAnimation(PickupGearInstancedWidgetA->FadeOutAnimation);
		}
	}
	ShowPickupAndInfoWidgets(false);

	// UE_LOG(LogTemp, Warning, TEXT("Exited proximity of weapon: %s"), *GetName());
}

void AAmmoPickup::SetAmmoPickupInformationText(UWidgetComponent* AmmoPickupWidgetComponent, AAmmoPickup* AmmoPickup)
{
	UItemInfoWidgetBase* AmmoWidget = Cast<UItemInfoWidgetBase>(AmmoPickupWidgetComponent->GetUserWidgetObject());
	if (AmmoPickup && AmmoWidget)
	{
		if (AmmoWidget->Line1) AmmoWidget->Line1->SetText(FText::FromString(AmmoPickup->AmmoName));
		if (AmmoWidget->Line2) AmmoWidget->Line2->SetText(FText::FromString(AmmoPickup->AmmoWeapon));
		if (AmmoWidget->Line3) AmmoWidget->Line3->SetText(FText::FromString(AmmoPickup->AmmoDeliverable));
		if (AmmoWidget->Line4) AmmoWidget->Line4->SetText(FText::FromString(AmmoPickup->AmmoAmount));
		if (AmmoWidget->Line5) AmmoWidget->Line5->SetText(FText::FromString(AmmoPickup->AmmoDamage));
	}
}

void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}
