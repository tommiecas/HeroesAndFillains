// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/Melee/MeleeWeapon.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/MeleeInfoWidgetComponent.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "WeaponsFinal/WeaponBase.h"
#include "HUD/ItemInfoWidgetBase.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(false, true);
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB->SetVisibility(false, true);
	}
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}


void AMeleeWeapon::ShowPickupAndInfoWidgets(bool bShowPickupAndInfoWidgets)
{
	Super::ShowPickupAndInfoWidgets(bShowPickupAndInfoWidgets);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (ItemInfoWidgetComponentA && !ItemInfoWidgetComponentA->GetWidgetClass())
	{
		ItemInfoWidgetComponentA->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentA->InitWidget();
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UUserWidget* Widget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(Widget))
		{
			InfoWidget->SetItemInformation(
				GetMeleeWeaponNameText(),
				GetMeleeWeaponHistoryText(),
				GetMeleeWeaponResistancesText(),
				GetMeleeWeaponWeaknessesText(),
				GetMeleeWeaponDamageText()
			);
		}
	}
	if (ItemInfoWidgetComponentB && !ItemInfoWidgetComponentB->GetWidgetClass())
	{
		ItemInfoWidgetComponentB->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentB->InitWidget();
		ItemInfoWidgetComponentB->SetVisibility(false, true);
		UUserWidget* Widget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(Widget))
		{
			InfoWidget->SetItemInformation(
				GetMeleeWeaponNameText(),
				GetMeleeWeaponHistoryText(),
				GetMeleeWeaponResistancesText(),
				GetMeleeWeaponWeaknessesText(),
				GetMeleeWeaponDamageText()
			);
		}
	}
	ShowPickupAndInfoWidgets(false);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in MeleeWeaponBeginPlay"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in MeleeWeaponBeginPlay"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void AMeleeWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowMeleeInfoFadeInAnimation(); 
			}
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB->SetVisibility(true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowMeleeInfoFadeInAnimation(); 
			}
		}
	}
	ShowPickupAndInfoWidgets(true);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in MeleeWeaponOnOverlap"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in MeleeWeaponOverlap"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void AMeleeWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowMeleeInfoFadeOutAnimation(); 
			}
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB->SetVisibility(false, true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowMeleeInfoFadeOutAnimation(); 
			}
		}
	}
	ShowPickupAndInfoWidgets(false);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in MeleeWeaponEndOverlap"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in MeleeWeaponEndOverlap"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void AMeleeWeapon::OnEquipped()
{
	Super::OnEquipped();
}

void AMeleeWeapon::OnDropped()
{
	Super::OnDropped();

	if (!ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetA"));
		ItemInfoWidgetComponentA->RegisterComponent();
		ItemInfoWidgetComponentA->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ItemInfoWidgetComponentA->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentA->InitWidget();

		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject()))
		{
			InfoWidget->SetItemInformation(
				GetMeleeWeaponNameText(),
				GetMeleeWeaponHistoryText(),
				GetMeleeWeaponResistancesText(),
				GetMeleeWeaponWeaknessesText(),
				GetMeleeWeaponDamageText()
			);
		}
	}
	if (!ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetB"));
		ItemInfoWidgetComponentB->RegisterComponent();
		ItemInfoWidgetComponentB->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ItemInfoWidgetComponentB->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentB->InitWidget();

		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentB->GetUserWidgetObject()))
		{
			InfoWidget->SetItemInformation(
				GetMeleeWeaponNameText(),
				GetMeleeWeaponHistoryText(),
				GetMeleeWeaponResistancesText(),
				GetMeleeWeaponWeaknessesText(),
				GetMeleeWeaponDamageText()
			);
		}
	}
	if (FloatingWidgetComponent)
	{
		FloatingWidgetComponent->SetVisibility(false, true);
	}
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in MeleeWeaponDropped"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in MeleeWeaponDropped"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void AMeleeWeapon::OnEquippedSecondary()
{
	Super::OnEquippedSecondary();
}

FText AMeleeWeapon::GetMeleeWeaponNameText()
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

void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
}

void AMeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

