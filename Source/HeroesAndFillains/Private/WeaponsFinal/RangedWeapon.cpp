// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/RangedWeapon.h"

#include "WeaponsFinal/WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponsFinal/CasingFinal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/RangedInfoWidgetComponent.h"

ARangedWeapon::ARangedWeapon()
	: Super()
{
	RangedInfoWidget1 = CreateDefaultSubobject<URangedInfoWidgetComponent>(TEXT("RangedInfoWidget1"));
	RangedInfoWidget1->SetupAttachment(RootComponent);
	RangedInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	RangedInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
	RangedInfoWidget1->SetWidgetClass(URangedInfoWidgetComponent::StaticClass()); // Set the widget class to your custom widget class

	RangedInfoWidget2 = CreateDefaultSubobject<URangedInfoWidgetComponent>(TEXT("RangedInfoWidget2"));
	RangedInfoWidget2->SetupAttachment(RootComponent);
	RangedInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	RangedInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
	RangedInfoWidget2->SetWidgetClass(URangedInfoWidgetComponent::StaticClass()); // Set the widget class to your custom widget classWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget2"));
}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangedWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ARangedWeapon::OnSphereEndOverlap);
	}
	
	if (RangedInfoWidget1)
	{
		FText TypeText = GetRangedWeaponTypeText();
		RangedInfoWidget1->SetRangedWeaponInfo(WeaponName, WeaponDescription, TypeText, WeaponRarity, WeaponDamage);
		RangedInfoWidget1->SetVisibility(false);
	}
	
	if (RangedInfoWidget2)
	{
		FText TypeText = GetRangedWeaponTypeText();
		RangedInfoWidget1->SetRangedWeaponInfo(WeaponName, WeaponDescription, TypeText, WeaponRarity, WeaponDamage);
		RangedInfoWidget2->SetVisibility(false);
	}
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
	
	if (RangedInfoWidget1)
	{
		RangedInfoWidget1->SetVisibility(true);
	}
	if (RangedInfoWidget2)
	{
		RangedInfoWidget2->SetVisibility(true);
	}
	
}

void ARangedWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappingCOmponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappingCOmponent, OtherActor, OtherComp, OtherBodyIndex);
	
	if (RangedInfoWidget1)
	{
		RangedInfoWidget1->SetVisibility(false);
	}
	if (RangedInfoWidget2)
	{
		RangedInfoWidget1->SetVisibility(false);
	}
}

void ARangedWeapon::OnEquipped()
{
	Super::OnEquipped();

	if (RangedInfoWidget1) { RangedInfoWidget1->DestroyComponent(); RangedInfoWidget1 = nullptr; }
	if (RangedInfoWidget2) { RangedInfoWidget2->DestroyComponent(); RangedInfoWidget2 = nullptr; }

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

	// Create RangedInfoWidget1
	if (!RangedInfoWidget1)
	{
		RangedInfoWidget1 = NewObject<URangedInfoWidgetComponent>(this, TEXT("RangedInfoWidget1"));
		if (RangedInfoWidget1)
		{
			RangedInfoWidget1->RegisterComponent();
			RangedInfoWidget1->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			RangedInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);
			RangedInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
			RangedInfoWidget1->SetWidgetClass(URangedInfoWidgetComponent::StaticClass());
                
			RangedInfoWidget1->SetRangedWeaponInfo(WeaponName, WeaponDescription, WeaponType, WeaponRarity, WeaponDamage);
		}
	}
	// Create RangedInfoWidget2
	if (!RangedInfoWidget2)
	{
		RangedInfoWidget2 = NewObject<URangedInfoWidgetComponent>(this, TEXT("RangedInfoWidget2"));
		if (RangedInfoWidget2)
		{
			RangedInfoWidget2->RegisterComponent();
			RangedInfoWidget2->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			RangedInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);
			RangedInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
			RangedInfoWidget2->SetWidgetClass(URangedInfoWidgetComponent::StaticClass());
			
			RangedInfoWidget2->SetRangedWeaponInfo(WeaponName, WeaponDescription, WeaponType, WeaponRarity, WeaponDamage);
		}
	}
	if (FloatingWidgetComponent)
	{
		FloatingWidgetComponent->SetVisibility(false);
	}
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

	if (RangedInfoWidget1) { RangedInfoWidget1->DestroyComponent(); RangedInfoWidget1 = nullptr; }
	if (RangedInfoWidget2) { RangedInfoWidget2->DestroyComponent(); RangedInfoWidget2 = nullptr; }
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

	if (RangedInfoWidget1)
	{
		RangedInfoWidget1->SetVisibility(bShowPickupAndInfoWidgets);
	}
	if (RangedInfoWidget2)
	{
		RangedInfoWidget2->SetVisibility(bShowPickupAndInfoWidgets);
	}
}

void ARangedWeapon::Fire(const FVector& HitTarget)
{
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

FText ARangedWeapon::GetRangedWeaponTypeText() const
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








