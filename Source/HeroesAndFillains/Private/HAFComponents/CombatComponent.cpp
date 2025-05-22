// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFComponents/CombatComponent.h"
#include "WeaponsFinal/WeaponBase.h"
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
#include "WeaponsFinal/Ranged/ProjectileFinal.h"
#include "WeaponsFinal/Ranged/Shotgun.h"
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "WeaponsFinal/Ranged/RangedWeapon.h"
#include "WeaponsFinal/Melee/MeleeWeapon.h"


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
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, Grenades);
	DOREPLIFETIME(UCombatComponent, bWieldingTheSword);
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
	if (FiringWeapon && FiringWeapon->IsRangedWeaponEmpty() && FiringWeapon->GetRangedWeaponType() == RangedType)
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
	// Character->GetFillainHUD()->SetCrosshairsSpread(CrosshairVelocityFactor, CrosshairInAirFactor, CrosshairAimFactor, CrosshairShootingFactor);
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AFillainHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
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


void UCombatComponent::FireButtonPressed(bool bPressed)
{
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
	if (Character && CombatState == ECombatState::ECS_Reloading && EquippedRangedWeapon->GetRangedWeaponType() == ERangedType::ERT_Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedRangedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedRangedWeapon->Fire(TraceHitTarget);
	}	
}

void UCombatComponent::Fire()
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

void UCombatComponent::FireProjectileWeapon()
{
	if (EquippedRangedWeapon && Character)
	{
		HitTarget = EquippedRangedWeapon->bUseScatter ? EquippedRangedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedRangedWeapon->FireDelay);
	}
}

void UCombatComponent::FireHitScanWeapon()
{
	if (EquippedRangedWeapon && Character)
	{
		HitTarget = EquippedRangedWeapon->bUseScatter ? EquippedRangedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedRangedWeapon->FireDelay);
	}
}

void UCombatComponent::FireShotgun()
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

void UCombatComponent::StartFireTimer()
{
	if (EquippedRangedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedRangedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedRangedWeapon == nullptr) return;
	bCanGunFire = true;
	if (bIsFireButtonPressed && EquippedRangedWeapon->bIsWeaponAutomatic)
	{
		Fire();
	}
	ReloadEmptyRangedWeapon();
}



void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	MulticastShotgunFire(TraceHitTargets);
}

bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (EquippedRangedWeapon)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedRangedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}

void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalShotgunFire(TraceHitTargets);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedRangedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedRangedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedRangedWeapon);
	if (Shotgun == nullptr || Character == nullptr) return;
	if (CombatState == ECombatState::ECS_Reloading || CombatState == ECombatState::ECS_Unoccupied)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		Shotgun->FireShotgun(TraceHitTargets);
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(WeaponToEquip))
	{
		DropEquippedWeapon();

		EquippedWeapon = WeaponToEquip;
		EquippedRangedWeapon = RangedWeapon;

		RangedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachWeaponToRightHand(WeaponToEquip);
		RangedWeapon->SetOwner(Character);
		PlayWeaponEquipSound(WeaponToEquip);
		RangedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();

		UE_LOG(LogTemp, Warning, TEXT("Equipping Weapon: %s"), *RangedWeapon->GetName());
	}
	else if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(WeaponToEquip))
	{
		DropEquippedWeapon();

		EquippedWeapon = WeaponToEquip;
		EquippedMeleeWeapon = MeleeWeapon;

		MeleeWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachWeaponToRightHand(WeaponToEquip);
		MeleeWeapon->SetOwner(Character);
		PlayWeaponEquipSound(WeaponToEquip);
	}
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedRangedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()];
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
	if (EquippedRangedWeapon && EquippedRangedWeapon->IsRangedWeaponEmpty())
	{
		Reload();
	}
}

void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr || !Character->HasAuthority()) return;

	Character->PlaySwapMontage();
	CombatState = ECombatState::ECS_SwappingWeapons;
	Character->bFinishedSwapping = false;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(false);

}

void UCombatComponent::EquipPrimaryWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->bShouldHover = false;
	EquippedWeapon->bShouldFloatSpin = false;
	EquippedWeapon->HoverLight->SetVisibility(false);
	EquippedWeapon->HoverDecal->SetVisibility(false);
	EquippedWeapon->ShowPickupAndInfoWidgets(false);
	PlayWeaponEquipSound(EquippedWeapon);
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
	{
		AttachWeaponToRightHand(WeaponToEquip);
		RangedWeapon->SetHUDAmmo();	
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
	}
	else if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		AttachWeaponToRightHand(WeaponToEquip);
	}
}

void UCombatComponent::EquipSecondaryWeapon(AWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);
	PlayWeaponEquipSound(SecondaryWeapon);
	SecondaryWeapon->SetOwner(Character);
	SecondaryWeapon->bShouldHover = false;
	SecondaryWeapon->bShouldFloatSpin = false;
	SecondaryWeapon->HoverLight->SetVisibility(false);
	SecondaryWeapon->HoverDecal->SetVisibility(false);
	SecondaryWeapon->ShowPickupAndInfoWidgets(false);
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::AttachActorToBackpack(AWeaponBase* WeaponToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || WeaponToAttach == nullptr) return;
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
	}
}

void UCombatComponent::AttachWeaponToRightHand(class AWeaponBase* WeaponToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || WeaponToAttach == nullptr) return;

	// Attach the MESH directly, not the root
	if (WeaponToAttach->WeaponMesh)
	{
		WeaponToAttach->WeaponMesh->AttachToComponent(
			Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			FName("RightHandSocket")
		);

		WeaponToAttach->WeaponMesh->SetVisibility(true, true);
		WeaponToAttach->WeaponMesh->SetHiddenInGame(false, true);
		WeaponToAttach->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	/* if (Character == nullptr || Character->GetMesh() == nullptr || WeaponToAttach == nullptr) return;

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(WeaponToAttach, Character->GetMesh());
	} */
}

void UCombatComponent::AttachWeaponToLeftHand(AWeaponBase* WeaponToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || WeaponToAttach == nullptr || EquippedWeapon == nullptr) return;
	if (EquippedRangedWeapon)
	{
		bool bUsePistolSocket = EquippedRangedWeapon->GetRangedWeaponType() == ERangedType::ERT_Pistol || EquippedRangedWeapon->GetRangedWeaponType() == ERangedType::ERT_SubmachineGun;
		FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);		
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToAttach, Character->GetMesh());
		}
	}
	else if (EquippedMeleeWeapon)
	{
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("LeftHandSocket"));				
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToAttach, Character->GetMesh());
		}
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied && EquippedRangedWeapon && !EquippedRangedWeapon->IsRangedWeaponFull() && !bLocallyReloading)
	{
		ServerReloading();
		HandleReload();
		bLocallyReloading = true;
	}
}

void UCombatComponent::ServerReloading_Implementation()
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Reloading;
	if (!Character->IsLocallyControlled()) HandleReload();
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	bLocallyReloading = false;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bIsFireButtonPressed)
	{
		Fire();
	}

}

void UCombatComponent::FinishSwap()
{
	if (Character && Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (Character) Character->bFinishedSwapping = true;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(true);

}

void UCombatComponent::FinishSwapAttachWeapons()
{
	AWeaponBase* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachWeaponToRightHand(EquippedWeapon);
	PlayWeaponEquipSound(EquippedWeapon);
	if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon))
	{
		RangedWeapon->SetHUDAmmo();
		UpdateCarriedAmmo();
	}

	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedRangedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedWeaponType()))
	{
		CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()];
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
	if (Character == nullptr || EquippedRangedWeapon == nullptr) return;

	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedWeaponType()))
	{
		CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()];
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
	CombatState = ECombatState::ECS_Unoccupied;
	AttachWeaponToRightHand(EquippedWeapon);
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
	if (Character && GrenadeFinalClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectileFinal>(
				GrenadeFinalClass,
				StartingLocation,
				ToTarget.Rotation(),
				SpawnParams
			);
		}
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled()) HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bIsFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachWeaponToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
		}
		break;
	case ECombatState::ECS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlaySwapMontage();
		}
		break;
	}
}

void UCombatComponent::HandleReload()
{
	if (Character)
	{
		Character->PlayReloadingMontage();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedRangedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedRangedWeapon->GetMagCapacity() - EquippedRangedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedRangedWeapon->GetRangedWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedRangedWeapon->GetRangedWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}

	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
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
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachWeaponToRightHand(EquippedWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedWeapon);
	}
}

void UCombatComponent::OnRep_EquippedMeleeWeapon()
{
	if (EquippedMeleeWeapon && Character)
	{
		EquippedMeleeWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachWeaponToRightHand(EquippedMeleeWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedMeleeWeapon);
	}
}

void UCombatComponent::OnRep_EquippedRangedWeapon()
{
	if (EquippedRangedWeapon && Character)
	{
		EquippedRangedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachWeaponToRightHand(EquippedRangedWeapon);
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
	if (Character->IsLocallyControlled() && EquippedRangedWeapon->GetRangedWeaponType() == ERangedType::ERT_SniperRifle)
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

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: ❌ CombatState is not ECS_Unoccupied"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("CanFire ✅ All checks passed"));
	return true;
	// return !EquippedRangedWeapon->IsRangedWeaponEmpty() && bCanGunFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotgunEnd = CombatState == ECombatState::ECS_Reloading && EquippedRangedWeapon != nullptr && EquippedRangedWeapon->GetRangedWeaponType() == ERangedType::ERT_Shotgun && CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
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
