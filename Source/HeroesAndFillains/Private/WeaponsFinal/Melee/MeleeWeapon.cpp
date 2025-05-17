// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/Melee/MeleeWeapon.h"
#include "Components/WidgetComponent.h"
#include "HUD/MeleeInfoWidgetComponent.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "WeaponsFinal/WeaponBase.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	MeleeInfoWidget1 = CreateDefaultSubobject<UMeleeInfoWidgetComponent>(TEXT("MeleeInfoWidget1"));
	MeleeInfoWidget1->SetupAttachment(RootComponent);
	MeleeInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	MeleeInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
	MeleeInfoWidget1->SetWidgetClass(UMeleeInfoWidgetComponent::StaticClass()); // Set the widget class to your custom widget class

	MeleeInfoWidget2 = CreateDefaultSubobject<UMeleeInfoWidgetComponent>(TEXT("MeleeInfoWidget2"));
	MeleeInfoWidget2->SetupAttachment(RootComponent);
	MeleeInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	MeleeInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
	MeleeInfoWidget2->SetWidgetClass(UMeleeInfoWidgetComponent::StaticClass()); // Set the widget class to your custom widget classWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget2"));
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}


void AMeleeWeapon::ShowPickupAndInfoWidgets(bool bShowPickupAndInfoWidgets)
{
	Super::ShowPickupAndInfoWidgets(bShowPickupAndInfoWidgets);

	if (MeleeInfoWidget1)
	{
		MeleeInfoWidget1->SetVisibility(bShowPickupAndInfoWidgets);
	}
	if (MeleeInfoWidget2)
	{
		MeleeInfoWidget2->SetVisibility(bShowPickupAndInfoWidgets);
	}
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (MeleeInfoWidget1)
	{
		FText NameText = GetMeleeWeaponNameText();
		MeleeInfoWidget1->SetMeleeWeaponInfo(NameText, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		MeleeInfoWidget1->SetVisibility(false);
	}

	if (MeleeInfoWidget2)
	{
		FText NameText = GetMeleeWeaponNameText();
		MeleeInfoWidget2->SetMeleeWeaponInfo(NameText, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		MeleeInfoWidget2->SetVisibility(false);
	}
}

void AMeleeWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (MeleeInfoWidget1)
	{
		MeleeInfoWidget1->SetVisibility(true);
	}
	if (MeleeInfoWidget2)
	{
		MeleeInfoWidget2->SetVisibility(true);
	}
}

void AMeleeWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (MeleeInfoWidget1)
	{
		MeleeInfoWidget1->SetVisibility(false);
	}
	if (MeleeInfoWidget2)
	{
		MeleeInfoWidget1->SetVisibility(false);
	}
}

FText AMeleeWeapon::GetMeleeWeaponNameText() const
{
	// Convert the EMeleeType enum value to a localized FText using the display name
	const UEnum* EnumPtr = StaticEnum<EMeleeType>();
	if (EnumPtr) 
	{
		// Get display name from enum (uses UMETA(DisplayName) if provided)
		return EnumPtr->GetDisplayNameTextByValue((int64)MeleeType);
	}
	// Fallback text if enum is invalid
	return FText::FromString("Unknown");;
}

void AMeleeWeapon::OnEquipped()
{
	Super::OnEquipped();

	if (MeleeInfoWidget1) { MeleeInfoWidget1->DestroyComponent(); MeleeInfoWidget1 = nullptr; }
	if (MeleeInfoWidget2) { MeleeInfoWidget2->DestroyComponent(); MeleeInfoWidget2 = nullptr; }
}

void AMeleeWeapon::OnDropped()
{
	Super::OnDropped();

	// Create MeleeInfoWidget1
	if (!MeleeInfoWidget1)
	{
		MeleeInfoWidget1 = NewObject<UMeleeInfoWidgetComponent>(this, TEXT("MeleeInfoWidget1"));
		if (MeleeInfoWidget1)
		{
			MeleeInfoWidget1->RegisterComponent();
			MeleeInfoWidget1->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			MeleeInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);
			MeleeInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
			MeleeInfoWidget1->SetWidgetClass(UMeleeInfoWidgetComponent::StaticClass());
                
			MeleeInfoWidget1->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		}
	}
	// Create MeleeInfoWidget2
	if (!MeleeInfoWidget2)
	{
		MeleeInfoWidget2 = NewObject<UMeleeInfoWidgetComponent>(this, TEXT("MeleeInfoWidget2"));
		if (MeleeInfoWidget2)
		{
			MeleeInfoWidget2->RegisterComponent();
			MeleeInfoWidget2->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			MeleeInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);
			MeleeInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
			MeleeInfoWidget2->SetWidgetClass(UMeleeInfoWidgetComponent::StaticClass());
			
			MeleeInfoWidget2->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		}
	}
	if (FloatingWidgetComponent)
	{
		FloatingWidgetComponent->SetVisibility(false);
	}
}

void AMeleeWeapon::OnEquippedSecondary()
{
	Super::OnEquippedSecondary();

	if (MeleeInfoWidget1) { MeleeInfoWidget1->DestroyComponent(); MeleeInfoWidget1 = nullptr; }
	if (MeleeInfoWidget2) { MeleeInfoWidget2->DestroyComponent(); MeleeInfoWidget2 = nullptr; }
}

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
}

void AMeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMeleeWeapon, MeleeType);
}

