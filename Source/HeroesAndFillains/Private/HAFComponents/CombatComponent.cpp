// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFComponents/CombatComponent.h"
#include "Weapons/Weapon.h"
#include "WeaponsFinal/WeaponFinal.h"
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
#include "Weapons/Projectile.h"
#include "WeaponsFinal/ProjectileFinal.h"
#include "Weapons/Shotgun.h"
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 800.f;
	AimWalkSpeed = 600.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeaponFinal);
	DOREPLIFETIME(UCombatComponent, SecondaryWeaponFinal);
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

void UCombatComponent::PickupAmmo(EWeaponFinalType FinalType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(FinalType))
	{
		CarriedAmmoMap[FinalType] = FMath::Clamp(CarriedAmmoMap[FinalType] + AmmoAmount, 0, MaxCarriedAmmo);
		
		UpdateCarriedAmmo();
	}
	if (EquippedWeaponFinal && EquippedWeaponFinal->IsWeaponFinalEmpty() && EquippedWeaponFinal->GetWeaponFinalType() == WeaponFinalType)
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
			if (EquippedWeaponFinal)
			{
				HUDPackage.CrosshairsCenter = EquippedWeaponFinal->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeaponFinal->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeaponFinal->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeaponFinal->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeaponFinal->CrosshairsTop;
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
	if (bIsFireButtonPressed)
	{
		Fire(); 
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget/*, float FireDelay*/)
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
	if (EquippedWeaponFinal == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Reloading && EquippedWeaponFinal->GetWeaponFinalType() == EWeaponFinalType::EWFT_Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeaponFinal->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeaponFinal->Fire(TraceHitTarget);
	}	
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanGunFire = false;
		ServerFire(HitTarget);
		if (EquippedWeaponFinal)
		{
			CrosshairShootingFactor = 0.75f;

			/*switch (EquippedWeapon->FireType)
			{
			case EFireType::EFT_Projectile:
				FireProjectileWeapon();
				break;
			case EFireType::EFT_HitScan:
				FireHitScanWeapon();
				break;
			case EFireType::EFT_Shotgun:
				FireShotgun();
				break;
			}*/
		}
		StartFireTimer();
	}
}

void UCombatComponent::FireProjectileWeapon()
{
	if (EquippedWeaponFinal && Character)
	{
		HitTarget = EquippedWeaponFinal->bUseScatter ? EquippedWeaponFinal->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget/*, EquippedWeapon->FireDelay*/);
	}
}

void UCombatComponent::FireHitScanWeapon()
{
	if (EquippedWeaponFinal && Character)
	{
		HitTarget = EquippedWeaponFinal->bUseScatter ? EquippedWeaponFinal->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget/*, EquippedWeapon->FireDelay*/);
	}
}

void UCombatComponent::FireShotgun()
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeaponFinal);
	if (Shotgun && Character)
	{
		TArray<FVector_NetQuantize> HitTargets;
		Shotgun->ShotgunTraceEndWithScatter(HitTarget, HitTargets);
		if (!Character->HasAuthority()) LocalShotgunFire(HitTargets);
		ServerShotgunFire(HitTargets, EquippedWeaponFinal->FireDelay);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeaponFinal == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeaponFinal->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeaponFinal == nullptr) return;
	bCanGunFire = true;
	if (bIsFireButtonPressed && EquippedWeaponFinal->bIsWeaponAutomatic)
	{
		Fire();
	}
	ReloadEmptyWeaponFinal();
}



void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	MulticastShotgunFire(TraceHitTargets);
}

bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (EquippedWeaponFinal)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeaponFinal->FireDelay, FireDelay, 0.001f);
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
	if (EquippedWeaponFinal == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeaponFinal->Fire(TraceHitTarget);
	}
}

void UCombatComponent::LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeaponFinal);
	if (Shotgun == nullptr || Character == nullptr) return;
	if (CombatState == ECombatState::ECS_Reloading || CombatState == ECombatState::ECS_Unoccupied)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		Shotgun->FireShotgun(TraceHitTargets);
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::EquipWeaponFinal(AWeaponFinal* WeaponFinalToEquip)
{
	if (Character == nullptr || WeaponFinalToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	DropEquippedWeaponFinal();
	EquippedWeaponFinal = WeaponFinalToEquip;
	EquippedWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_Equipped);
	AttachActorToRightHand(EquippedWeaponFinal);
	EquippedWeaponFinal->SetOwner(Character);
	EquippedWeaponFinal->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayWeaponFinalEquipSound(WeaponFinalToEquip);
	ReloadEmptyWeaponFinal();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeaponFinal == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeaponFinal->GetWeaponFinalType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()];
	}

	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayWeaponFinalEquipSound(AWeaponFinal* WeaponFinalToEquip)
{
	if (Character && EquippedWeaponFinal && EquippedWeaponFinal->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeaponFinal->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::ReloadEmptyWeaponFinal()
{
	if (EquippedWeaponFinal && EquippedWeaponFinal->IsWeaponFinalEmpty())
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
	if (SecondaryWeaponFinal) SecondaryWeaponFinal->EnableCustomDepth(false);

}

void UCombatComponent::EquipPrimaryWeaponFinal(AWeaponFinal* WeaponFinalToEquip)
{
	if (WeaponFinalToEquip == nullptr) return;
	DropEquippedWeaponFinal();
	EquippedWeaponFinal = WeaponFinalToEquip;
	EquippedWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_Equipped);
	AttachActorToRightHand(EquippedWeaponFinal);
	EquippedWeaponFinal->SetOwner(Character);
	EquippedWeaponFinal->SetHUDAmmo();
	EquippedWeaponFinal->bShouldHover = false;
	EquippedWeaponFinal->bShouldFloatSpin = false;
	EquippedWeaponFinal->HoverLight->SetVisibility(false);
	EquippedWeaponFinal->HoverDecal->SetVisibility(false);
	EquippedWeaponFinal->ShowPickupAndWeaponInfoWidgets(false);
	UpdateCarriedAmmo();
	PlayWeaponFinalEquipSound(WeaponFinalToEquip);
	ReloadEmptyWeaponFinal();

}

void UCombatComponent::EquipSecondaryWeaponFinal(AWeaponFinal* WeaponFinalToEquip)
{
	if (WeaponFinalToEquip == nullptr) return;
	SecondaryWeaponFinal = WeaponFinalToEquip;
	SecondaryWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_EquippedSecondary);
	AttachActorToBackpack(WeaponFinalToEquip);
	PlayWeaponFinalEquipSound(WeaponFinalToEquip);
	SecondaryWeaponFinal->SetOwner(Character);
	SecondaryWeaponFinal->bShouldHover = false;
	SecondaryWeaponFinal->bShouldFloatSpin = false;
	SecondaryWeaponFinal->HoverLight->SetVisibility(false);
	SecondaryWeaponFinal->HoverDecal->SetVisibility(false);
	SecondaryWeaponFinal->ShowPickupAndWeaponInfoWidgets(false);
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::AttachActorToBackpack(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* BackpackSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (BackpackSocket)
	{
		BackpackSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}
void UCombatComponent::DropEquippedWeaponFinal()
{
	if (EquippedWeaponFinal)
	{
		EquippedWeaponFinal->WeaponFinalDropped();
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr || EquippedWeaponFinal == nullptr) return;
	bool bUsePistolSocket = EquippedWeaponFinal->GetWeaponFinalType() == EWeaponFinalType::EWFT_Pistol || EquippedWeaponFinal->GetWeaponFinalType() == EWeaponFinalType::EWFT_SubmachineGun;
	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied && EquippedWeaponFinal && !EquippedWeaponFinal->IsWeaponFinalFull() && !bLocallyReloading)
	{
		ServerReloading();
		HandleReload();
		bLocallyReloading = true;
	}
}

void UCombatComponent::ServerReloading_Implementation()
{
	if (Character == nullptr || EquippedWeaponFinal == nullptr) return;

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
	if (SecondaryWeaponFinal) SecondaryWeaponFinal->EnableCustomDepth(true);

}

void UCombatComponent::FinishSwapAttachWeapons()
{
	AWeaponFinal* TempWeaponFinal = EquippedWeaponFinal;
	EquippedWeaponFinal = SecondaryWeaponFinal;
	SecondaryWeaponFinal = TempWeaponFinal;

	EquippedWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_Equipped);
	AttachActorToRightHand(EquippedWeaponFinal);
	EquippedWeaponFinal->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayWeaponFinalEquipSound(EquippedWeaponFinal);

	SecondaryWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeaponFinal);

}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedWeaponFinal == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeaponFinal->GetWeaponFinalType()))
	{
		CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()];
	}
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeaponFinal->AddAmmo(ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (Character == nullptr || EquippedWeaponFinal == nullptr) return;

	if (CarriedAmmoMap.Contains(EquippedWeaponFinal->GetWeaponFinalType()))
	{
		CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()];
	}
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeaponFinal->AddAmmo(1);
	bCanGunFire = true;
	if (EquippedWeaponFinal->IsWeaponFinalFull() || CarriedAmmo == 0)
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
	AttachActorToRightHand(EquippedWeaponFinal);
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
			AttachActorToLeftHand(EquippedWeaponFinal);
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
	if (EquippedWeaponFinal == nullptr) return 0;
	int32 RoomInMag = EquippedWeaponFinal->GetMagCapacity() - EquippedWeaponFinal->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeaponFinal->GetWeaponFinalType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeaponFinal->GetWeaponFinalType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}

	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeaponFinal == nullptr) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeaponFinal);
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
		AttachActorToLeftHand(EquippedWeaponFinal);
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
	return (EquippedWeaponFinal != nullptr && SecondaryWeaponFinal != nullptr);
}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

void UCombatComponent::OnRep_EquippedWeaponFinal()
{
	if (EquippedWeaponFinal && Character)
	{
		EquippedWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_Equipped);
		AttachActorToRightHand(EquippedWeaponFinal);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponFinalEquipSound(EquippedWeaponFinal);
	}
}

void UCombatComponent::OnRep_SecondaryWeaponFinal()
{
	if (SecondaryWeaponFinal && Character)
	{
		SecondaryWeaponFinal->SetWeaponFinalState(EWeaponFinalState::EWFS_EquippedSecondary);
		AttachActorToBackpack(SecondaryWeaponFinal);
		PlayWeaponFinalEquipSound(EquippedWeaponFinal);
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
	if (EquippedWeaponFinal == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeaponFinal->GetZoomedFOV(), DeltaTime, EquippedWeaponFinal->GetZoomInterpSpeed());
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
	if (Character == nullptr || EquippedWeaponFinal == nullptr) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	if (Character->IsLocallyControlled() && EquippedWeaponFinal->GetWeaponFinalType() == EWeaponFinalType::EWFT_SniperRifle)
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
	if (EquippedWeaponFinal == nullptr) return false;
	return !EquippedWeaponFinal->IsWeaponFinalEmpty() && bCanGunFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotgunEnd = CombatState == ECombatState::ECS_Reloading && EquippedWeaponFinal != nullptr && EquippedWeaponFinal->GetWeaponFinalType() == EWeaponFinalType::EWFT_Shotgun && CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_SniperRifle, StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponFinalType::EWFT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
}

void UCombatComponent::OnRep_WieldingTheSword()
{
	if (bWieldingTheSword && Character && Character->IsLocallyControlled())
	{
		Character->Crouch();
	}
}
