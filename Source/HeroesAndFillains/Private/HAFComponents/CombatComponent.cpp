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
#include "Components/WidgetComponent.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/WeaponBase.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Enemies/EnemyBase.h"
#include "HeroesAndFillains/DebugMacros.h"
#include "Weapons/Majix/MajixWeapon.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	BaseWalkSpeed = 800.f;
	AimWalkSpeed = 600.f;

}

void UCombatComponent::SetCharacter(AFillainCharacter* InCharacter)
{
	Character = InCharacter;
	UE_LOG(LogTemp, Warning, TEXT("✅ CombatComponent: Character set to %s"), *GetNameSafe(Character));
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
	DOREPLIFETIME(UCombatComponent, CurrentHitAssistPaddingCM);

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
	// UCombatComponent ctor or BeginPlay()
	DefaultFOV = 90.f;
	CurrentFOV = DefaultFOV;

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(DefaultFOV);
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
			if (EquippedRangedWeapon)
			{
				APlayerController* PC = Cast<APlayerController>(GetCharacter()->GetController());
				if (PC)
				{
					PC->bEnableMouseOverEvents = false;         // Enable mouse over events
					PC->bShowMouseCursor = false;               // (Optional) Show the mouse if you're using it
				} 
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			if (EquippedWeapon)
			{
				APlayerController* PC = Cast<APlayerController>(GetCharacter()->GetController());
				if (PC)
				{
					PC->bEnableMouseOverEvents = false;         // Enable mouse over events
					PC->bShowMouseCursor = false;               // (Optional) Show the mouse if you're using it
				} 
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
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

void UCombatComponent::TraceForCrossHairTarget()
{
	constexpr float TraceDistance = 10000.0f; // 100 meters
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D CrosshairPosition(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	APlayerController* PC = Cast<APlayerController>(GetCharacter()->GetController());
	if (PC)
	{
		FVector WorldLocation;
		FVector WorldDirection;

		FVector2D ScreenPosition = CrosshairPosition; // usually center of screen

		bool bSuccess = PC->DeprojectScreenPositionToWorld(
			ScreenPosition.X,
			ScreenPosition.Y,
			WorldLocation,
			WorldDirection
		);

		if (bSuccess)
		{
			// Do your line trace here using WorldLocation and WorldDirection
			FVector TraceStart = WorldLocation;
			FVector TraceEnd = TraceStart + WorldDirection * TraceDistance;
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetCharacter());

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
        
			AEnemyBase* NewEnemy = nullptr;
			if (bHit)
			{
				NewEnemy = Cast<AEnemyBase>(HitResult.GetActor());
			}

			// Highlighting logic
			if (NewEnemy != HighlightedEnemy)
			{
				if (HighlightedEnemy)
				{
					HighlightedEnemy->UnHighlightActor(); // Replace with your unhighlight function
				}

				HighlightedEnemy = NewEnemy;

				if (HighlightedEnemy)
				{
					HighlightedEnemy->HighlightActor(); // Replace with your highlight function
				}
			}
		}
	}
}

void UCombatComponent::Server_CacheHitAssistPadding_Implementation()
{
	if (AFillainCharacter* OwnerChar = Cast<AFillainCharacter>(GetOwner()))
	{
		CurrentHitAssistPaddingCM = OwnerChar->GetHitAssistPaddingCM();
		UE_LOG(LogTemp, Verbose, TEXT("[Assist] Cached Vision pad = %.1f cm"), CurrentHitAssistPaddingCM);
	}
}

void UCombatComponent::FireButtonPressed(const bool bPressed)
{
	if (GetOwnerRole() == ROLE_Authority) Server_CacheHitAssistPadding();
	else Server_CacheHitAssistPadding(); // RPC to server
	if (FightingStyle != EFightingStyle::EFS_Ranged || FightingStyle != EFightingStyle::EFS_Melee || FightingStyle != EFightingStyle::EFS_Majix) return;
	if (FightingStyle == EFightingStyle::EFS_Ranged)
	{
		bIsFireButtonPressed = bPressed;

		// UE_LOG(LogTemp, Warning, TEXT("CombatComponent::FireButtonPressed called: %s"), bPressed ? TEXT("true") : TEXT("false"));

		if (bIsFireButtonPressed)
		{
			Fire(); 
		}
	}
	if (FightingStyle == EFightingStyle::EFS_Melee)
	{
		Character->MeleeAttack();
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
	if (GetOwnerRole() == ROLE_Authority) Server_CacheHitAssistPadding();
	else Server_CacheHitAssistPadding();
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
	if (CanFire())
	{
		if (GetOwnerRole() == ROLE_Authority) Server_CacheHitAssistPadding();
		else Server_CacheHitAssistPadding();
		// UE_LOG(LogTemp, Warning, TEXT("CanFire() == true"));
		bCanGunFire = false;
		if (EquippedRangedWeapon)
		{
			// UE_LOG(LogTemp, Warning, TEXT("EquippedRangedWeapon: %s, FireType: %d"),
			//	*EquippedRangedWeapon->GetName(), (int32)EquippedRangedWeapon->FireType);

			CrosshairShootingFactor = 0.75f;

			switch (EquippedRangedWeapon->FireType)
			{
			case EFireType::EFT_Projectile:
				// UE_LOG(LogTemp, Warning, TEXT("Calling FireProjectileWeapon()"));
				FireProjectileWeapon();
				break;
			case EFireType::EFT_HitScan:
				// UE_LOG(LogTemp, Warning, TEXT("Calling FireHitScanWeapon()"));
				FireHitScanWeapon();
				break;
			case EFireType::EFT_Shotgun:
				// UE_LOG(LogTemp, Warning, TEXT("Calling FireShotgun()"));
				FireShotgun();
				break;
			}
		}		
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("CanFire() == false"));
		}
		StartFireTimer();
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

void UCombatComponent::ReceiveMeleeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser,
	const FVector& WeaponOrigin,
	const FVector& HitLocation
)
{
	// Apply actual damage
	float FinalDamage = Character->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (FinalDamage <= 0.f)
	{
		return;
	}
	
	// Optional: cache for post-hit logic
	Character->CachedDamageAmount = FinalDamage;
	Character->CachedDamageEvent = DamageEvent;
	Character->CachedEventInstigator = EventInstigator;
	Character->CachedDamageCauser = DamageCauser;

	Character->DirectionalHitReact(HitLocation);

	
	
	Character->GetHit_Implementation(HitLocation, DamageCauser);
	
	// Optional: spawn effects like blood, sparks, decals
	Character->PlayHitSound(HitLocation);
	Character->SpawnHitSpecialEffects(HitLocation);

	if (bRecentlyDamaged) return;
	bRecentlyDamaged = true;

	// Reset in 0.2s
	GetWorld()->GetTimerManager().SetTimer(RecentDamageHandle, this, &UCombatComponent::ResetRecentlyDamaged, 0.2f, false);
	
	UE_LOG(LogTemp, Warning, TEXT("🎯 ReceiveMeleeDamage called on %s for %.1f damage"), *Character->GetName(), DamageAmount);
}

void UCombatComponent::ResetRecentlyDamaged()
{
	bRecentlyDamaged = false;
}

void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	/* EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(Character->GetOverlappingItem()), *GetNameSafe(Character->GetOverlappingWeapon()));*/

	if (!WeaponToEquip || !Character) return;

	if (CurrentlyEquippedWeapon == WeaponToEquip)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Weapon %s is already equipped."), *WeaponToEquip->GetName());
		return;
	}

	if (EquippedWeapon) DropEquippedWeapon();
	
	EquippedWeapon = WeaponToEquip;

	USkeletalMeshComponent* CharMesh = Character ? Character->GetMesh() : nullptr;
	const FName EquipSocket = FName(TEXT("RangedSocket")); // or whatever you actually use

	if (ARangedWeapon* EquippedRanged = Cast<ARangedWeapon>(EquippedWeapon))
	{
		EquippedRangedWeapon = EquippedRanged;
		EquippedRangedWeapon->SetEquippedRangedWeaponState();
		EquippedRangedWeapon->Equip(CharMesh, EquipSocket, GetOwner(), GetCharacter());
		FightingStyle = EFightingStyle::EFS_Ranged;
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
		bWieldingTheSword = false;
		EquippedRangedWeapon->bIsEquipped = true;
	}
	if (AMeleeWeapon* EquippedMelee = Cast<AMeleeWeapon>(EquippedWeapon))
	{
		EquippedMeleeWeapon = EquippedMelee;
		EquippedMeleeWeapon->SetEquippedMeleeWeaponState();
		EquippedMeleeWeapon->Equip(CharMesh, EquipSocket, GetOwner(), GetCharacter());
		FightingStyle = EFightingStyle::EFS_Ranged;
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
		bWieldingTheSword = false;
		EquippedMeleeWeapon->bIsEquipped = true;
		if (EquippedWeapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
		{
			Character->EquipOneHandedMeleeWeapon(EquippedWeapon);
		}
		if (EquippedWeapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
		{
			Character->EquipTwoHandedMeleeWeapon(EquippedWeapon);
		}
	}
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

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	Character->PlaySwapMontage();
	ActionState = EActionState::EAS_SwappingWeapons;
	Character->bFinishedSwapping = false;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(false);
	ActionState = EActionState::EAS_Unoccupied;

}

void UCombatComponent::EquipPrimaryWeapon(AWeaponBase* WeaponToEquip)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(Character->GetOverlappingItem()), *GetNameSafe(Character->GetOverlappingWeapon()));
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
	SetHandsForWeapons(EquippedWeapon);
	if (EquippedWeapon->IsA(ARangedWeapon::StaticClass()))
	{
		ARangedWeapon* Ranged = Cast<ARangedWeapon>(EquippedWeapon);
		FightingStyle = EFightingStyle::EFS_Ranged;
		Ranged->SetHUDAmmo();	
		UpdateCarriedAmmo();
		ReloadEmptyRangedWeapon();
		ActionState = EActionState::EAS_Unoccupied;
	}
	else if (EquippedWeapon->IsA(AMeleeWeapon::StaticClass()))
	{
		AMeleeWeapon* Melee = Cast<AMeleeWeapon>(EquippedWeapon);
		FightingStyle = EFightingStyle::EFS_Melee;
		ActionState = EActionState::EAS_Unoccupied;
		if (Melee->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon) Character->BattlePrepped = EBattlePrepped::EBP_ArmedOneHandedMeleeWeapon;
		if (Melee->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon) Character->BattlePrepped = EBattlePrepped::EBP_ArmedTwoHandedMeleeWeapon;
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
		EquippedWeapon->WeaponState = EWeaponState::EWS_Dropped;
		EquippedWeapon->WeaponDropped();
		FightingStyle = EFightingStyle::EFS_Unequipped;
		
	}
}

void UCombatComponent::AttachWeaponToSocket(AWeaponBase* Weapon, FName SocketName)
{
	if (!AreMeshesValid(Weapon)) return;
	if (!Character)
	{
		// UE_LOG(LogTemp, Error, TEXT("AttachWeaponToSocket failed: Character is null."));
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		// UE_LOG(LogTemp, Error, TEXT("AttachWeaponToSocket failed: Character mesh is null."));
		return;
	}

	if (!Mesh->DoesSocketExist(SocketName))
	{
		// UE_LOG(LogTemp, Error, TEXT("Socket '%s' does not exist."), *SocketName.ToString());
		return;
	}
	
	
	// 🧪 Attach using KeepWorld so actor stays at the transform we just set
	FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
	Weapon->AttachToComponent(Mesh, AttachRules, SocketName);

	// DEBUG: Log final transforms
	// UE_LOG(LogTemp, Warning, TEXT("Weapon Actor Transform AFTER attach: %s"), *Weapon->GetActorTransform().ToString());

	// Cleanup visual/collision
	Weapon->SetActorHiddenInGame(false);
	Weapon->SetActorEnableCollision(false);
	Weapon->WeaponMesh->SetVisibility(true);
	Weapon->WeaponMesh->SetHiddenInGame(false);
	Weapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// UE_LOG(LogTemp, Warning, TEXT("Sword Mesh Relative Loc: %s"), *Weapon->WeaponMesh->GetRelativeLocation().ToString());
}

void UCombatComponent::AttachOneHandedRangedWeaponToRightHand(class AWeaponBase* OneHandedRangedWeaponToAttach)
{
	FName SocketName = (OneHandedRangedWeaponToAttach->GetWeaponType() == EWeaponType::EWT_AssaultRifle)
			? FName("AssaultRifleSocket")
			: FName("RangedSocket");
	AttachWeaponToSocket(OneHandedRangedWeaponToAttach, SocketName);
}

void UCombatComponent::AttachOneHandedMeleeWeaponToRightHand(class AWeaponBase* OneHandedMeleeWeaponToAttach)
{
	OneHandedMeleeWeaponToAttach->Equip(Character->GetMesh(), FName("MeleeSocket"), OneHandedMeleeWeaponToAttach->GetOwner(), Character);
}

void UCombatComponent::AttachTwoHandedMeleeWeaponToLeftHand(AWeaponBase* TwoHandedMeleeWeaponToAttach)
{
	// Just attach to right hand - FABRIK will handle the left hand
	AttachOneHandedMeleeWeaponToRightHand(TwoHandedMeleeWeaponToAttach);
    
	// Remove this line:
	// EquippedMeleeWeapon->AttachMeshToSocket(Character->GetMesh(), FName("LeftHandSocket"));
}

void UCombatComponent::AttachTwoHandedRangedWeaponToLeftHand(class AWeaponBase* TwoHandedRangedWeaponToAttach)
{
	AttachOneHandedRangedWeaponToRightHand(TwoHandedRangedWeaponToAttach);
}


void UCombatComponent::AttachWeaponToLeftHand(AWeaponBase* WeaponToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || WeaponToAttach == nullptr || EquippedWeapon == nullptr) return;

	bool bUsePistolSocket = 

		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||

		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SubmachineGun;

	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);

	if (HandSocket)

	{

		HandSocket->AttachActor(WeaponToAttach, Character->GetMesh());

	}
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
	if (!Character) return;

	// Ensure we're still in swapping state before changing it
	if (ActionState == EActionState::EAS_SwappingWeapons)
	{
		if (Character->HasAuthority())
		{
			ActionState = EActionState::EAS_Unoccupied;
		}
        
		if (Character)
		{
			Character->bFinishedSwapping = true;
		}
        
		if (SecondaryWeapon)
		{
			SecondaryWeapon->EnableCustomDepth(true);
		}
	}

}

void UCombatComponent::FinishSwapAttachWeapons()
{
	AWeaponBase* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;
	
	EquippedWeapon->SetEquippedWeaponState();
	SetFightingStyle();
	PlayWeaponEquipSound(EquippedWeapon);
	SetHandsForWeapons(EquippedWeapon);
	if (EquippedWeapon->IsA(ARangedWeapon::StaticClass()))
	{
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
		if (RangedWeapon)
		{
			RangedWeapon->SetHUDAmmo();
			UpdateCarriedAmmo();
		}
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
	SetHandsForWeapons(EquippedWeapon);
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
			Character->ResetToFightAgain();
		}
		break;
	case EActionState::EAS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
			Character->PlayThrowGrenadeMontage();
			AttachWeaponToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
			Character->ResetToFightAgain();
		}
		break;
	case EActionState::EAS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
			Character->PlaySwapMontage();
			Character->ResetToFightAgain();
		}
		break;
	case EActionState::EAS_MeleeAttacking:
		if (Character && !Character->IsLocallyControlled())
		{
			UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
			ActionState = EActionState::EAS_MeleeAttacking;
			Character->MeleeAttack();
			Character->ResetToFightAgain();
		}
	}
}

void UCombatComponent::HandleReload()
{
	if (Character && Character->PlayerUsingRangedWeapons())
	{
		Character->PlayReloadingMontage();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (Character->PlayerNotUsingRangedWeapons()) return 0;
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
		UAnimInstance* AnimInstance = nullptr;
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
		Character->ResetToFightAgain();
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
		Character->ResetToFightAgain();
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
		Character->ResetToFightAgain();
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	ActionState = EActionState::EAS_ThrowingGrenade;
	if (Character)
	{
		UAnimInstance* AnimInstance = nullptr;
		Character->PlayThrowGrenadeMontage();
		AttachWeaponToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
	Character->ResetToFightAgain();
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
	Character->ResetToFightAgain();
}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
}

EFightingStyle UCombatComponent::SetFightingStyle()
{
	if (Character->EquippedWeaponIsARangedWeapon())
	{
		FightingStyle = EFightingStyle::EFS_Ranged;
	}
	else if (Character->EquippedWeaponIsAMeleeWeapon())
	{
		FightingStyle = EFightingStyle::EFS_Melee;
	}
	else if (Character->EquippedWeaponIsAMajixWeapon())
	{
		FightingStyle = EFightingStyle::EFS_Majix;
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

bool UCombatComponent::EquippedWeaponUsesOneHand(AWeaponBase* WeaponEquipping)
{
	return WeaponEquipping->WeaponState == EWeaponState::EWS_EquippedOneHanded;
}

void UCombatComponent::SetHandsForWeapons(AWeaponBase* WeaponEquipping)
{
	if (Character->EquippedWeaponIsARangedWeapon())
	{
		if (EquippedWeaponUsesOneHand(WeaponEquipping))
		{
			AttachOneHandedRangedWeaponToRightHand(WeaponEquipping);			
		}
		else if (WeaponEquipping->WeaponState == EWeaponState::EWS_EquippedTwoHanded)
		{
			AttachTwoHandedRangedWeaponToLeftHand(WeaponEquipping);
		}
	}
	else if (WeaponEquipping->IsA(AMeleeWeapon::StaticClass()))
	{
		if (WeaponEquipping->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
		{
			WeaponEquipping->WeaponState = EWeaponState::EWS_EquippedOneHanded;
			AttachOneHandedMeleeWeaponToRightHand(WeaponEquipping);
		}
		else if (WeaponEquipping->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
		{
			WeaponEquipping->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
			AttachTwoHandedMeleeWeaponToLeftHand(WeaponEquipping);
		}
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (AFillainCharacter* FC = Cast<AFillainCharacter>(GetOwner()))
	{
		FC->StartCamWatchdog(2.0f, 0.05f);
	}
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(Character->GetOverlappingItem()), *GetNameSafe(Character->GetOverlappingWeapon()));
	if (AFillainCharacter* FC = Cast<AFillainCharacter>(GetOwner()))
	{
		FC->StartCamWatchdog(2.0f, 0.05f);   // <— add this too
	}
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetEquippedWeaponState();
		SetHandsForWeapons(EquippedWeapon);	
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedWeapon);
		SetFightingStyle();
	}
	Character->StartCamWatchdog(2.f);
}

void UCombatComponent::OnRep_EquippedMeleeWeapon()
{
	if (EquippedMeleeWeapon && Character)
	{
		FightingStyle = EFightingStyle::EFS_Melee;
		EquippedMeleeWeapon->SetEquippedWeaponState();
		SetHandsForWeapons(EquippedMeleeWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedMeleeWeapon);
	}
}

void UCombatComponent::OnRep_EquippedMajixWeapon()
{
	if (EquippedMajixWeapon && Character)
	{
		FightingStyle = EFightingStyle::EFS_Majix;
		EquippedMajixWeapon->SetEquippedWeaponState();
		SetHandsForWeapons(EquippedMajixWeapon);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedMajixWeapon);
	}
}

void UCombatComponent::OnRep_EquippedRangedWeapon()
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(Character->GetOverlappingItem()), *GetNameSafe(Character->GetOverlappingWeapon()));

	if (EquippedRangedWeapon && Character)
	{
		FightingStyle = EFightingStyle::EFS_Ranged;
		EquippedRangedWeapon->SetEquippedRangedWeaponState();
		if (EquippedRangedWeapon->WeaponState == EWeaponState::EWS_EquippedOneHanded)
		{
			AttachOneHandedRangedWeaponToRightHand(EquippedRangedWeapon);
		}
		else if (EquippedRangedWeapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded)
		{
			AttachTwoHandedRangedWeaponToLeftHand(EquippedRangedWeapon);
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayWeaponEquipSound(EquippedRangedWeapon);
	}
	Character->StartCamWatchdog(2.f);
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
	FVector2D ViewportSize = FVector2D::ZeroVector;
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
	if (!EquippedRangedWeapon || !Character || !Character->GetFollowCamera()) return;

	const float MinFOV = 45.f;
	const float MaxFOV = 120.f;

	const float TargetFOV = FMath::Clamp(
		bAiming ? EquippedRangedWeapon->GetZoomedFOV() : DefaultFOV,
		MinFOV, MaxFOV);

	const float Speed = bAiming ? EquippedRangedWeapon->GetZoomInterpSpeed() : ZoomInterpSpeed;

	// Clamp input & output so a bad default can’t nuke the camera
	CurrentFOV = FMath::Clamp(CurrentFOV, MinFOV, MaxFOV);
	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, Speed);

	Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
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