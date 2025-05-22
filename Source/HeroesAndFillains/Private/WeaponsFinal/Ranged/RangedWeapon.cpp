// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/Ranged/RangedWeapon.h"

#include "WeaponsFinal/WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponsFinal/Ranged/CasingFinal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/RangedInfoWidgetComponent.h"

ARangedWeapon::ARangedWeapon()
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

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (ItemInfoWidgetComponentA && !ItemInfoWidgetComponentA->GetWidgetClass())
	{
		ItemInfoWidgetComponentA->SetWidgetClass(ItemInfoWidgetClass); // ← assign it here!
		ItemInfoWidgetComponentA->InitWidget();
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UItemInfoWidgetBase* InfoWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (InfoWidgetA)
		{
			InfoWidgetA->SetItemInformation(
				GetRangedWeaponNameText(),
				GetRangedWeaponDescriptionText(),
				GetRangedWeaponTypeText(),
				GetRangedWeaponRarityText(),
				GetRangedWeaponDamageText()
			);
			if (ItemInfoWidgetComponentB && !ItemInfoWidgetComponentB->GetWidgetClass())
			{
				ItemInfoWidgetComponentB->SetWidgetClass(ItemInfoWidgetClass);
				ItemInfoWidgetComponentB->InitWidget();
				ItemInfoWidgetComponentB->SetVisibility(false, true);
				UItemInfoWidgetBase* InfoWidgetB = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentB->GetUserWidgetObject());
				if (InfoWidgetB)
				{
					InfoWidgetB->SetItemInformation(
						GetRangedWeaponNameText(),
						GetRangedWeaponDescriptionText(),
						GetRangedWeaponTypeText(),
						GetRangedWeaponRarityText(),
						GetRangedWeaponDamageText()
					);
				}
			}
			ShowPickupAndInfoWidgets(false);
			const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
			const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
			UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s RangedWeaponBeginPlay"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
			UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s RangedWeaponBeginPlay"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Widget A visibility in game: %s"), ItemInfoWidgetComponentA->IsVisible() ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("Widget B visibility in game: %s"), ItemInfoWidgetComponentB->IsVisible() ? TEXT("Visible") : TEXT("Hidden"));
}

void ARangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARangedWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ARangedWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

void ARangedWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				InfoWidget->ShowRangedInfoFadeInAnimation(); 
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
				InfoWidget->ShowRangedInfoFadeInAnimation(); 
			}
		}
	}
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in RangedWeaponOverlap RangedWeaponOnOverlap"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in RangedWeaponOverlapRangedWeaponOnOverlap"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void ARangedWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappingComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowRangedInfoFadeOutAnimation(); 
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
				InfoWidget->ShowRangedInfoFadeOutAnimation(); 
			}
		}
	}
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in RangedWEaponEndOverlap"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in RangedWeaponEndOverlap"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void ARangedWeapon::OnEquipped()
{
	Super::OnEquipped();
	
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter && bUseServerSideRewind)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && !FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.AddDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
}

void ARangedWeapon::OnDropped()
{
	Super::OnDropped();

	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.RemoveDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
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
				GetRangedWeaponNameText(),
				GetRangedWeaponDescriptionText(),
				GetRangedWeaponTypeText(),
				GetRangedWeaponRarityText(),
				GetRangedWeaponDamageText()
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
			GetRangedWeaponNameText(),
				GetRangedWeaponDescriptionText(),
				GetRangedWeaponTypeText(),
				GetRangedWeaponRarityText(),
				GetRangedWeaponDamageText()
			);
		}
	}
	if (FloatingWidgetComponent)
	{
		FloatingWidgetComponent->SetVisibility(false, true);
	}
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in RangedWeaponDropped"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in RangedWeaponDropped"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}

void ARangedWeapon::OnEquippedSecondary()
{
	Super::OnEquippedSecondary();

	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.RemoveDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
	
}

void ARangedWeapon::SetHUDAmmo()
{
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;


	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		
		if (FillainOwnerController)
		{
			FillainOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void ARangedWeapon::SpendRoundOfAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void ARangedWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void ARangedWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void ARangedWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter && FillainOwnerCharacter->GetCombatComponent() && IsRangedWeaponFull())
	{
		FillainOwnerCharacter->GetCombatComponent()->JumpToShotgunEnd();
	}
	SetHUDAmmo();
}

void ARangedWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner != nullptr)
	{
		FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(Owner) : FillainOwnerCharacter;
		if (FillainOwnerCharacter && FillainOwnerCharacter->GetEquippedWeapon() && FillainOwnerCharacter->GetEquippedWeapon() == Cast<AWeaponBase>(this))
		{
			SetHUDAmmo();
		}
	}
}

void ARangedWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

void ARangedWeapon::ShowPickupAndInfoWidgets(bool bShowPickupAndInfoWidgets)
{
	Super::ShowPickupAndInfoWidgets(bShowPickupAndInfoWidgets);
}

void ARangedWeapon::Fire(const FVector& HitTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("ARangedWeapon::Fire called on %s"), *GetName());

	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingFinalClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasingFinal>(
					CasingFinalClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	SpendRoundOfAmmo();
}

bool ARangedWeapon::IsRangedWeaponEmpty()
{
	return (Ammo <= 0);
}

bool ARangedWeapon::IsRangedWeaponFull()
{
	return Ammo == MagCapacity;
}

FText ARangedWeapon::GetRangedWeaponTypeText()
{
	// Convert the ERangedType enum value to a localized FText using the display name
	const UEnum* EnumPtr = StaticEnum<ERangedType>();
	if (EnumPtr) 
	{
		// Get display name from enum (uses UMETA(DisplayName) if provided)
		return EnumPtr->GetDisplayNameTextByValue((int64)RangedType);
	}
	// Fallback text if enum is invalid
	return FText::FromString("Unknown");;;
}

FVector ARangedWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlashSocket");
	if (MuzzleFlashSocket == nullptr) return FVector();
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	/* DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Cyan,
		true);*/

	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}









