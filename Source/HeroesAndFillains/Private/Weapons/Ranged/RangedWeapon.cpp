// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ranged/RangedWeapon.h"

#include "Weapons/WeaponBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/Ranged/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/WeaponTypes.h"
#include "HUD/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "Components/TextBlock.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponTypes.h"

ARangedWeapon::ARangedWeapon()
	: Super()
{
	// Initialize the WeaponAmmoMap
	RangedWeaponAmmoMap.Add(ERangedType::ERT_AssaultRifle,    EAmmoType::EAT_ARAmmo);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_RocketLauncher,  EAmmoType::EAT_Rockets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Pistol,          EAmmoType::EAT_Bullets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SubmachineGun,   EAmmoType::EAT_Magazines);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Shotgun,         EAmmoType::EAT_Shells);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_GrenadeLauncher, EAmmoType::EAT_LaunchedGrenades);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SniperRifle,     EAmmoType::EAT_SniperAmmo);
}

TMap<ERangedType, EAmmoType> ARangedWeapon::RangedWeaponAmmoMap;

void ARangedWeapon::BuildWeaponAmmoMapIfNeeded()
{
	if (RangedWeaponAmmoMap.Num() > 0) return;

	RangedWeaponAmmoMap.Add(ERangedType::ERT_AssaultRifle,    EAmmoType::EAT_ARAmmo);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_RocketLauncher,  EAmmoType::EAT_Rockets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Pistol,          EAmmoType::EAT_Bullets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SubmachineGun,   EAmmoType::EAT_Magazines);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Shotgun,         EAmmoType::EAT_Shells);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_GrenadeLauncher, EAmmoType::EAT_LaunchedGrenades);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SniperRifle,     EAmmoType::EAT_SniperAmmo);
}

const TMap<ERangedType, EAmmoType>& ARangedWeapon::GetWeaponAmmoMap()
{
	BuildWeaponAmmoMapIfNeeded();
	return RangedWeaponAmmoMap;
}

bool ARangedWeapon::TryGetRangedTypeForAmmo(EAmmoType Ammo, ERangedType& OutRangedType)
{
	BuildWeaponAmmoMapIfNeeded();

	for (const TPair<ERangedType, EAmmoType>& Pair : RangedWeaponAmmoMap)
	{
		if (Pair.Value == Ammo)
		{
			OutRangedType = Pair.Key;
			return true;
		}
	}
	return false;
}

ERangedType GetWeaponForAmmo(EAmmoType AmmoType, const TMap<ERangedType, EAmmoType>& RangedWeaponAmmoMap)
{
	for (const TPair<ERangedType, EAmmoType>& Pair : RangedWeaponAmmoMap)
	{
		if (Pair.Value == AmmoType)
		{
			return Pair.Key; // Found the weapon that uses this ammo
		}
	}

	// Default fallback if not found
	return ERangedType::ERT_None; // Or a special "None" enum if you add one
}

void ARangedWeapon::SetEquippedRangedWeaponState()
{
	if (RangedType == ERangedType::ERT_RocketLauncher || RangedType == ERangedType::ERT_GrenadeLauncher || RangedType == ERangedType::ERT_SniperRifle || RangedType == ERangedType::ERT_Shotgun)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
	if (RangedType == ERangedType::ERT_AssaultRifle || RangedType == ERangedType::ERT_SubmachineGun || RangedType == ERangedType::ERT_Pistol)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();
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

void ARangedWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
}


void ARangedWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappingComponent, OtherActor, OtherComp, OtherBodyIndex);
	
}

void ARangedWeapon::OnEquippedOneHanded()
{
	Super::OnEquippedOneHanded();
	
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

void ARangedWeapon::OnEquippedTwoHanded()
{
	Super::OnEquippedTwoHanded();
	
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


void ARangedWeapon::Fire(const FVector& HitTarget)
{
	if (!IsValid(WeaponMesh)) return; // More robust check

	if (!IsValid(GetWorld())) return; // Add world validity check
    
	// UE_LOG(LogTemp, Warning, TEXT("ARangedWeapon::Fire called on %s"), *GetName());

	if (FireAnimation && IsValid(WeaponMesh))
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	if (CasingClass && IsValid(WeaponMesh))
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	SpendRoundOfAmmo();
}

void ARangedWeapon::Equip(USceneComponent* InParent, FName InSocketName,  AActor* NewOwner, APawn* NewInstigator)
{
	if (!InParent) return;
    
	// Make sure we're attaching to the skeletal mesh
	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(InParent))
	{
		if (!SkeletalMesh->DoesSocketExist(InSocketName))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist on parent mesh"), *InSocketName.ToString());
			return;
		}
        
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, 
											   EAttachmentRule::SnapToTarget, 
											   EAttachmentRule::SnapToTarget, 
											   true);
		WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
        
		// Debug log the attachment
		FTransform SocketTransform = SkeletalMesh->GetSocketTransform(InSocketName);
		// UE_LOG(LogTemp, Warning, TEXT("Attaching to socket %s at location: %s"), 
			//   *InSocketName.ToString(), 
			 //  *SocketTransform.GetLocation().ToString());
	}
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

void ARangedWeapon::SetRangedWeaponInformationText(UWidgetComponent* RangedWidgetComponent,
	ARangedWeapon* RangedWeapon)
{
	UItemInfoWidgetBase* RangedWidget = Cast<UItemInfoWidgetBase>(RangedWidgetComponent->GetUserWidgetObject());
	if (RangedWeapon && RangedWidget)
	{
		if (RangedWidget->Line1) RangedWidget->Line1->SetText(FText::FromString(RangedWeapon->RangedWeaponName));
		if (RangedWidget->Line2) RangedWidget->Line2->SetText(FText::FromString(RangedWeapon->RangedWeaponDescription));
		if (RangedWidget->Line3) RangedWidget->Line3->SetText(FText::FromString(RangedWeapon->RangedWeaponType));
		if (RangedWidget->Line4) RangedWidget->Line4->SetText(FText::FromString(RangedWeapon->RangedWeaponRarity));
		if (RangedWidget->Line5) RangedWidget->Line5->SetText(FText::FromString(RangedWeapon->RangedWeaponDamage));
	}
}