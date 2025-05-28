// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFComponents/CombatComponent.h"
#include "Weapons/WeaponBase.h"
#include "Characters/FillainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PlayerController/FIllainPlayerController.h"
#include "Camera/CameraComponent.h"
#include "HUD/FillainHUD.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Characters/FillainAnimInstance.h"
#include "Weapons/Ranged/Projectile.h"
#include "Weapons/Ranged/Shotgun.h"
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponTypes.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 800.f;
	AimWalkSpeed = 600.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedMeleeWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedRangedWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, ActionState);
	DOREPLIFETIME(UCombatComponent, Grenades);
	DOREPLIFETIME(UCombatComponent, bWieldingTheSword);
}

bool UCombatComponent::AreMeshesValid(AWeaponBase* Weapon) const
{
	return Character &&
		  Character->GetMesh() &&
		  Weapon &&
		  Weapon->WeaponMesh;
}

void UCombatComponent::ShotgunShellReload()
{
	if (Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UCombatComponent::PickupAmmo(ERangedType Type, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(Type))
	{
		CarriedAmmoMap[Type] = FMath::Clamp(CarriedAmmoMap[Type] + AmmoAmount, 0, MaxCarriedAmmo);
		
		UpdateCarriedAmmo();
	}
	ARangedWeapon* FiringWeapon = Cast<ARangedWeapon>(EquippedWeapon);
	if (FiringWeapon && FiringWeapon->IsRangedWeaponEmpty() && FiringWeapon->GetRangedType() == RangedType)
	{
		Reload();
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		} 
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{

		/* HitTarget = HitResult.ImpactPoint;*/

		if (Character && Character->IsLocallyControlled())
		{
			FHitResult HitResult;
			TraceUnderCrosshairs(HitResult);
			HitTarget = HitResult.ImpactPoint;

			SetHUDCrosshairs(DeltaTime);
			InterpFOV(DeltaTime);
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (EquippedMeleeWeapon) return;

	
	// Character->GetFillainHUD()->SetCrosshairsSpread(CrosshairVelocityFactor, CrosshairInAirFactor, CrosshairAimFactor, CrosshairShootingFactor);
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AFillainHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedRangedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedRangedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedRangedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedRangedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedRangedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedRangedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}
	
			// Calculate Crossshair Spread

			// [0,800] -> [0,1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, .58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
} 


void UCombatComponent::FireButtonPressed(const bool bPressed)
{
	if (FightingStyle != EFightingStyle::EFS_Ranged) return;
	bIsFireButtonPressed = bPressed;

	UE_LOG(LogTemp, Warning, TEXT("CombatComponent::FireButtonPressed called: %s"), bPressed ? TEXT("true") : TEXT("false"));

	if (bIsFireButtonPressed)
	{
		Fire(); 
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	MulticastFire(TraceHitTarget);
}

/* bool UCombatComponent::ServerFire_Validate(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (EquippedWeapon)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}*/

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character && ActionState == EActionState::EAS_Reloading && RangedType == ERangedType::ERT_Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedRangedWeapon->Fire(TraceHitTarget);
		ActionState = EActionState::EAS_Unoccupied;
		return;
	}
	if (Character && ActionState == EActionState::EAS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedRangedWeapon->Fire(TraceHitTarget);
	}	
}

void UCombatComponent::Fire()
{
	if (FightingStyle != EFightingStyle::EFS_Ranged) return;
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatComponent::Fire() called"));

		if (CanFire())
		{
			UE_LOG(LogTemp, Warning, TEXT("CanFire() == true"));
			bCanGunFire = false;
			if (EquippedRangedWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("EquippedRangedWeapon: %s, FireType: %d"),
					*EquippedRangedWeapon->GetName(), (int32)EquippedRangedWeapon->FireType);

				CrosshairShootingFactor = 0.75f;

				switch (EquippedRangedWeapon->FireType)
				{
				case EFireType::EFT_Projectile:
					UE_LOG(LogTemp, Warning, TEXT("Calling FireProjectileWeapon()"));
					FireProjectileWeapon();
					break;
				case EFireType::EFT_HitScan:
					UE_LOG(LogTemp, Warning, TEXT("Calling FireHitScanWeapon()"));
					FireHitScanWeapon();
					break;
				case EFireType::EFT_Shotgun:
					UE_LOG(LogTemp, Warning, TEXT("Calling FireShotgun()"));
					FireShotgun();
					break;
				}
			}	
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CanFire() == false"));
			}
			StartFireTimer();
		}
	}
}

void UCombatComponent::FireProjectileWeapon()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon && Character)
		{
			HitTarget = EquippedRangedWeapon->bUseScatter ? EquippedRangedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
			if (!Character->HasAuthority()) LocalFire(HitTarget);
			ServerFire(HitTarget, EquippedRangedWeapon->FireDelay);
		}
	}
}

void UCombatComponent::FireHitScanWeapon()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon && Character)
		{
			HitTarget = EquippedRangedWeapon->bUseScatter ? EquippedRangedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
			if (!Character->HasAuthority()) LocalFire(HitTarget);
			ServerFire(HitTarget, EquippedRangedWeapon->FireDelay);
		}
	}
}

void UCombatComponent::FireShotgun()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		AShotgun* Shotgun = Cast<AShotgun>(EquippedRangedWeapon);
		if (Shotgun && Character)
		{
			TArray<FVector_NetQuantize> HitTargets;
			Shotgun->ShotgunTraceEndWithScatter(HitTarget, HitTargets);
			if (!Character->HasAuthority()) LocalShotgunFire(HitTargets);
			ServerShotgunFire(HitTargets, EquippedRangedWeapon->FireDelay);
		}
	}
}

void UCombatComponent::StartFireTimer()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon == nullptr || Character == nullptr) return;
		Character->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UCombatComponent::FireTimerFinished,
			EquippedRangedWeapon->FireDelay
		);
	}
}

void UCombatComponent::FireTimerFinished()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon == nullptr) return;
		bCanGunFire = true;
		if (bIsFireButtonPressed && EquippedRangedWeapon->bIsWeaponAutomatic)
		{
			Fire();
		}
		ReloadEmptyRangedWeapon();
	}
}



void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		MulticastShotgunFire(TraceHitTargets);
	}
}

bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (EquippedRangedWeapon && FightingStyle == EFightingStyle::EFS_Ranged)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedRangedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}


void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
		LocalShotgunFire(TraceHitTargets);
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon == nullptr) return;
		if (Character && ActionState == EActionState::EAS_Unoccupied)
		{
			Character->PlayFireMontage(bAiming);
			EquippedRangedWeapon->Fire(TraceHitTarget);
		}
	}
}

void UCombatComponent::LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		AShotgun* Shotgun = Cast<AShotgun>(EquippedRangedWeapon);
		if (Shotgun == nullptr || Character == nullptr) return;
		if (ActionState == EActionState::EAS_Reloading || ActionState == EActionState::EAS_Unoccupied)
		{
			bLocallyReloading = false;
			Character->PlayFireMontage(bAiming);
			Shotgun->FireShotgun(TraceHitTargets);
			ActionState = EActionState::EAS_Unoccupied;
		}
	}
}

void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (!WeaponToEquip || !Character) return;

	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;

	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(WeaponToEquip))
	{
		EquippedRangedWeapon = RangedWeapon;
		FightingStyle = EFightingStyle::EFS_Ranged;
		RangedWeapon->SetEquippedRangedWeaponState();
		AttachRangedWeaponToRightHand(RangedWeapon);
		RangedWeapon->SetOwner(Character);
		PlayWeaponEquipSound(RangedWeapon);
		RangedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
		UE_LOG(LogTemp, Warning, TEXT("Equipping Ranged Weapon: %s"), *RangedWeapon->GetName());
	}
	else if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(WeaponToEquip))
	{
		EquippedMeleeWeapon = MeleeWeapon;
		FightingStyle = EFightingStyle::EFS_Melee;
		MeleeWeapon->SetEquippedWeaponState();
		AttachMeleeWeaponToRightHand(MeleeWeapon);
		MeleeWeapon->SetOwner(Character);
		PlayWeaponEquipSound(MeleeWeapon);
		UE_LOG(LogTemp, Warning, TEXT("Equipping Melee Weapon: %s"), *MeleeWeapon->GetName());
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedRangedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()];
	}

	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayWeaponEquipSound(AWeaponBase* WeaponToEquip)
{
	if (Character && EquippedWeapon && EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::ReloadEmptyRangedWeapon()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		if (EquippedRangedWeapon && EquippedRangedWeapon->IsRangedWeaponEmpty())
		{
			Reload();
		}
	}
}

void UCombatComponent::SwapWeapons()
{
	if (ActionState != EActionState::EAS_Unoccupied || Character == nullptr || !Character->HasAuthority()) return;

	Character->PlaySwapMontage();
	ActionState = EActionState::EAS_SwappingWeapons;
	Character->bFinishedSwapping = false;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(false);
	ActionState = EActionState::EAS_Unoccupied;

}

void UCombatComponent::EquipPrimaryWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	FightingStyle = EFightingStyle::EFS_Unequipped;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetEquippedWeaponState();
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->bShouldHover = false;
	EquippedWeapon->bShouldFloatSpin = false;
	EquippedWeapon->HoverLight->SetVisibility(false);
	EquippedWeapon->HoverDecal->SetVisibility(false);
	EquippedWeapon->ShowPickupAndInfoWidgets(false);
	PlayWeaponEquipSound(EquippedWeapon);
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
	{
		FightingStyle = EFightingStyle::EFS_Ranged;
		AttachRangedWeaponToRightHand(WeaponToEquip);
		RangedWeapon->SetHUDAmmo();	
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
		ActionState = EActionState::EAS_Unoccupied;
	}
	else if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		FightingStyle = EFightingStyle::EFS_Melee;
		AttachMeleeWeaponToRightHand(WeaponToEquip);
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void UCombatComponent::EquipSecondaryWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);
	PlayWeaponEquipSound(SecondaryWeapon);
	SetFightingStyle();
	SecondaryWeapon->SetOwner(Character);
	SecondaryWeapon->bShouldHover = false;
	SecondaryWeapon->bShouldFloatSpin = false;
	SecondaryWeapon->HoverLight->SetVisibility(false);
	SecondaryWeapon->HoverDecal->SetVisibility(false);
	SecondaryWeapon->ShowPickupAndInfoWidgets(false);
	ActionState = EActionState::EAS_Unoccupied;
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled() && FightingStyle == EFightingStyle::EFS_Ranged)
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::AttachActorToBackpack(AWeaponBase* WeaponToAttach)
{
	if (!AreMeshesValid(WeaponToAttach)) return;
	const USkeletalMeshSocket* BackpackSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (BackpackSocket)
	{
		BackpackSocket->AttachActor(WeaponToAttach, Character->GetMesh());
	}
}
void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->WeaponDropped();
		FightingStyle = EFightingStyle::EFS_Unequipped;
	}
}

void UCombatComponent::AttachWeaponToSocket(AWeaponBase* Weapon, FName SocketName)
{
	if (!AreMeshesValid(Weapon)) return;

	const USkeletalMeshSocket* Socket = Character->GetMesh()->GetSocketByName(SocketName);
	if (Socket)
	{
		Socket->AttachActor(Weapon, Character->GetMesh());

		Weapon->WeaponMesh->SetVisibility(true);
		Weapon->WeaponMesh->SetHiddenInGame(false);
		Weapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void UCombatComponent::AttachRangedWeaponToRightHand(class AWeaponBase* WeaponToAttach)
{
	if (!AreMeshesValid(WeaponToAttach)) return;

	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(WeaponToAttach);
	if (!RangedWeapon) return;

	FName SocketName = (RangedWeapon->GetRangedType() == ERangedType::ERT_AssaultRifle)
		? FName("AssaultRifleSocket")
		: FName("RightHandSocket");

	AttachWeaponToSocket(RangedWeapon, SocketName);
}

void UCombatComponent::AttachMeleeWeaponToRightHand(AWeaponBase* WeaponToAttach)
{
	if (!AreMeshesValid(WeaponToAttach)) return;

	AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(WeaponToAttach);
	if (!MeleeWeapon) return;

	AttachWeaponToSocket(MeleeWeapon, FName("MeleeSocket"));
}

void UCombatComponent::AttachWeaponToLeftHand(AWeaponBase* WeaponToAttach)
{
	if (!AreMeshesValid(WeaponToAttach)) return;

	FName SocketName = FName("LeftHandSocket");

	if (EquippedRangedWeapon)
	{
		bool bUsePistolSocket =
			EquippedRangedWeapon->GetRangedType() == ERangedType::ERT_Pistol ||
			EquippedRangedWeapon->GetRangedType() == ERangedType::ERT_SubmachineGun;

		SocketName = bUsePistolSocket ? FName("PistolSocket") : SocketName;
	}

	AttachWeaponToSocket(WeaponToAttach, SocketName);
}

void UCombatComponent::Reload()
{
	if (FightingStyle == EFightingStyle::EFS_Ranged && CarriedAmmo > 0 && ActionState == EActionState::EAS_Unoccupied && EquippedRangedWeapon && !EquippedRangedWeapon->IsRangedWeaponFull() && !bLocallyReloading)
	{
		ServerReloading();
		HandleReload();
		bLocallyReloading = true;
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void UCombatComponent::ServerReloading_Implementation()
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr || FightingStyle != EFightingStyle::EFS_Ranged) return;

	ActionState = EActionState::EAS_Reloading;
	if (!Character->IsLocallyControlled()) HandleReload();
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr || FightingStyle != EFightingStyle::EFS_Ranged) return;
	bLocallyReloading = false;
	if (Character->HasAuthority())
	{
		ActionState = EActionState::EAS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bIsFireButtonPressed)
	{
		ActionState = EActionState::EAS_Unoccupied;
		Fire();
	}

}

void UCombatComponent::FinishSwap()
{
	if (Character && Character->HasAuthority())
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
	if (Character) Character->bFinishedSwapping = true;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(true);

}

void UCombatComponent::FinishSwapAttachWeapons()
{
	AWeaponBase* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;
	
	EquippedWeapon->SetEquippedWeaponState();
	SetFightingStyle();
	PlayWeaponEquipSound(EquippedWeapon);
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
	{
		AttachRangedWeaponToRightHand(EquippedWeapon);
		RangedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
	}
	if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		AttachMeleeWeaponToRightHand(EquippedWeapon);
	}

	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr || FightingStyle != EFightingStyle::EFS_Ranged) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedType()))
	{
		CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()];
	}
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedRangedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr || FightingStyle != EFightingStyle::EFS_Ranged) return;

	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedType()))
	{
		CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()];
	}
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedRangedWeapon->AddAmmo(1);
	bCanGunFire = true;
	if (EquippedRangedWeapon->IsRangedWeaponFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::JumpToShotgunEnd()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadingMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	ActionState = EActionState::EAS_Unoccupied;
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
	{
		AttachRangedWeaponToRightHand(EquippedWeapon);
	}
	if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		AttachMeleeWeaponToRightHand(EquippedWeapon);
	}
}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (Character && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				GrenadeClass,
				StartingLocation,
				ToTarget.Rotation(),
				SpawnParams
			);
		}
	}
}

void UCombatComponent::OnRep_ActionState()
{
	switch (ActionState)
	{
	case EActionState::EAS_Reloading:
		if (Character && !Character->IsLocallyControlled()) HandleReload();
		ActionState = EActionState::EAS_Reloading;
		break;
	case EActionState::EAS_Unoccupied:
		if (bIsFireButtonPressed)
		{
			Fire();
			ActionState = EActionState::EAS_Unoccupied;
		}
		break;
	case EActionState::EAS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachWeaponToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
			ActionState = EActionState::EAS_Unoccupied;
		}
		break;
	case EActionState::EAS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlaySwapMontage();
			ActionState = EActionState::EAS_Unoccupied;
		}
		break;
		case EActionState::EAS_MeleeAttacking:
		if (Character && !Character->IsLocallyControlled())
		{
			ActionState = EActionState::EAS_MeleeAttacking;
			Character->MeleeAttack();
			ActionState = EActionState::EAS_Unoccupied;
		}
	}
}

void UCombatComponent::HandleReload()
{
	if (Character && FightingStyle == EFightingStyle::EFS_Ranged)
	{
		Character->PlayReloadingMontage();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedRangedWeapon == nullptr || FightingStyle != EFightingStyle::EFS_Ranged) return 0;
	int32 RoomInMag = EquippedRangedWeapon->GetMagCapacity() - EquippedRangedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedRangedWeapon->GetRangedType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}

	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (ActionState != EActionState::EAS_Unoccupied || EquippedWeapon == nullptr) return;
	ActionState = EActionState::EAS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
		ActionState = EActionState::EAS_Unoccupied;
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
		ActionState = EActionState::EAS_Unoccupied;
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	ActionState = EActionState::EAS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
	ActionState = EActionState::EAS_Unoccupied;
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
	ActionState = EActionState::EAS_Unoccupied;
}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
}

EFightingStyle UCombatComponent::SetFightingStyle()
{
	if (ARangedWeapon* EquippedGun = Cast<ARangedWeapon>(EquippedWeapon))
	{
		FightingStyle = EFightingStyle::EFS_Ranged;
	}
	else if (AMeleeWeapon* EquippedMelee = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		FightingStyle = EFightingStyle::EFS_Melee;
	}
	else if (!EquippedWeapon)
	{
		FightingStyle = EFightingStyle::EFS_Unequipped;
	}
	else
	{
		FightingStyle = EFightingStyle::EFS_MAX;
	}
	return FightingStyle;

}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetEquippedWeaponState();
		if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
		{
			AttachRangedWeaponToRightHand(EquippedWeapon);
		}
		if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon))
		{
			AttachMeleeWeaponToRightHand(EquippedWeapon);
		}	
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedWeapon);
		SetFightingStyle();
	}
}

void UCombatComponent::OnRep_EquippedMeleeWeapon()
{
	if (EquippedMeleeWeapon && Character)
	{
		FightingStyle = EFightingStyle::EFS_Melee;
		EquippedMeleeWeapon->SetEquippedWeaponState();
		AttachMeleeWeaponToRightHand(EquippedMeleeWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedMeleeWeapon);
	}
}

void UCombatComponent::OnRep_EquippedRangedWeapon()
{
	if (EquippedRangedWeapon && Character)
	{
		FightingStyle = EFightingStyle::EFS_Ranged;
		EquippedRangedWeapon->SetEquippedRangedWeaponState();
		AttachRangedWeaponToRightHand(EquippedRangedWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedRangedWeapon);
	}
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackpack(SecondaryWeapon);
		PlayWeaponEquipSound(EquippedWeapon);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		} 
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedRangedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedRangedWeapon->GetZoomedFOV(), DeltaTime, EquippedRangedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	if (Character->IsLocallyControlled() && EquippedRangedWeapon->GetRangedType() == ERangedType::ERT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	} 
	if (Character->IsLocallyControlled()) bAimButtonPressed = bIsAiming; 
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedRangedWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ EquippedRangedWeapon == nullptr"));
		return false;
	}
	if (EquippedRangedWeapon->IsRangedWeaponEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ Weapon is empty"));
		return false;
	}

	if (!bCanGunFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ bCanGunFire == false"));
		return false;
	}

	if (ActionState != EActionState::EAS_Unoccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ ActionState is not EAS_Unoccupied"));
		ActionState = EActionState::EAS_Unoccupied;
		return true;
	}

	if (FightingStyle != EFightingStyle::EFS_Ranged)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ FightingStyle is not EFS_Ranged"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("CanFire ✅ All checks passed"));
	return true;
	// return !EquippedRangedWeapon->IsRangedWeaponEmpty() && bCanGunFire && ActionState == EActionState::EAS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotgunEnd = ActionState == EActionState::EAS_Reloading && EquippedRangedWeapon != nullptr && EquippedRangedWeapon->GetRangedType() == ERangedType::ERT_Shotgun && CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(ERangedType::ERT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_SniperRifle, StartingSniperAmmo);
	CarriedAmmoMap.Emplace(ERangedType::ERT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
}

void UCombatComponent::OnRep_WieldingTheSword()
{
	if (bWieldingTheSword && Character && Character->IsLocallyControlled())
	{
		Character->Crouch();
	}
}
