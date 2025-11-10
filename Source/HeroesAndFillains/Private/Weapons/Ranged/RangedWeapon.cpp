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
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "UI/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "Components/TextBlock.h"
#include "HAFComponents/CombatComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "UI/ItemInfoWidgetBase.h"
#include "UI/PickupGearWidget.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/CharacterTypes.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Enemies/EnemyBase.h"
#include "UI/ItemInfoWidgetBase.h"

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

	if (IsValid(ItemInfoWidgetComponent))
	{
		ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);        // or World
		ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
		if (ItemInfoWidgetClass)
		{
			ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
		}
	}
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
	if (RangedType == ERangedType::ERT_AssaultRifle || RangedType == ERangedType::ERT_RocketLauncher || RangedType == ERangedType::ERT_GrenadeLauncher || RangedType == ERangedType::ERT_SniperRifle || RangedType == ERangedType::ERT_Shotgun)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
	if (RangedType == ERangedType::ERT_SubmachineGun || RangedType == ERangedType::ERT_Pistol)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
}

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();

		
	SetEquippedRangedWeaponState();
	if (WeaponState == EWeaponState::EWS_EquippedOneHanded) HighPingOnEquippedOneHanded();
	if (WeaponState == EWeaponState::EWS_EquippedTwoHanded) HighPingOnEquippedTwoHanded();
	if (WeaponState == EWeaponState::EWS_EquippedSecondary) HighPingOnEquippedSecondary();
	if (WeaponState == EWeaponState::EWS_Dropped) HighPingOnDropped();;
	if (WeaponState == EWeaponState::EWS_Unclaimed) return;

	SetRangedWeaponInformationText(GetItemInfoWidgetComponent(), this);
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

void ARangedWeapon::HighPingOnEquippedOneHanded()
{
	
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter && bUseServerSideRewind)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && !FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.AddDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
	OnRep_Owner();
}

void ARangedWeapon::HighPingOnEquippedTwoHanded()
{
	
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter && bUseServerSideRewind)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && !FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.AddDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
	OnRep_Owner();
}

void ARangedWeapon::HighPingOnEquippedSecondary()
{

	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.RemoveDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
	OnRep_Owner();
}

void ARangedWeapon::HighPingOnDropped()
{
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (!AreaSphere)
	{
		AreaSphere = NewObject<USphereComponent>(this, TEXT("Area Sphere"));
		AreaSphere->RegisterComponent();
		AreaSphere->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	WeaponMesh->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = true;
			bShouldFloatSpin = true;
			WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
			WeaponMesh->MarkRenderStateDirty();
			EnableCustomDepth(true);
	
			HoverDecal->SetVisibility(true);
			HoverLight->SetVisibility(true);
		},
		0.1f,
		false);
	
	if (!IsValid(this)) return;
    
	if (!PickupGearWidgetComponent && !IsValid(PickupGearWidgetComponent))
	{
		PickupGearWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		if (PickupGearWidgetComponent)
		{
			PickupGearWidgetComponent->RegisterComponent();
			PickupGearWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupGearWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupGearWidgetComponent->InitWidget();
			PickupGearWidgetComponent->SetVisibility(true);
			PickupGearWidgetComponent->SetTickWhenOffscreen(true);
			PickupGearWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupGearWidgetComponent->TranslucencySortPriority = 5;
			if (auto* W = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject()))
			{
				W->PickupGearOwningComponent = PickupGearWidgetComponent;
			}
		}
		UPickupGearWidget* PickupWidget = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupWidget)
		{
			PickupGearWidgetComponent = PickupWidget->GetOwningWidgetComponent();
		}
	}
	
	if (!IsValid(this)) return;
    
	if (!ItemInfoWidgetComponent && !IsValid(ItemInfoWidgetComponent))
	{
		ItemInfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		if (ItemInfoWidgetComponent)
		{
			ItemInfoWidgetComponent->RegisterComponent();
			ItemInfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ItemInfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			ItemInfoWidgetComponent->InitWidget();
			ItemInfoWidgetComponent->SetVisibility(true);
			ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			ItemInfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			ItemInfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
			ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			ItemInfoWidgetComponent->SetVisibility(true);
			ItemInfoWidgetComponent->SetTickWhenOffscreen(true);
			ItemInfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			ItemInfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			ItemInfoWidgetComponent->TranslucencySortPriority = 5;
			if (auto* IIW = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject()))
			{
				IIW->ItemInfoOwningComponent = ItemInfoWidgetComponent;
			}
		}
	}
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;
		if (FillainOwnerController && HasAuthority() && FillainOwnerController->HighPingDelegate.IsBound())
		{
			FillainOwnerController->HighPingDelegate.RemoveDynamic(this, &ARangedWeapon::OnPingTooHigh);
		}
	}
	OnRep_Owner();
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
		if (!SkeletalMesh->DoesSocketExist(InSocketName)) return;
		if (NewOwner->ActorHasTag(FName("Player")))
		{
			const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RangedSocket"));
			if (HandSocket)
			{
				HandSocket->AttachActor(Character->CombatComponent->GetEquippedRangedWeapon(), Character->GetMesh());
				Super::Equip(Character->GetMesh(), FName("RangedSocket"), NewOwner, NewInstigator);
			}
		}
		else if (NewOwner->ActorHasTag(FName("Enemy")))
		{
			ABaseCharacter* BaseChar = Cast<ABaseCharacter>(NewOwner);
			const USkeletalMeshSocket* HandSocket = BaseChar->GetMesh()->GetSocketByName(FName("RangedSocket"));
			if (HandSocket)
			{
				AEnemyBase* EnChar = Cast<AEnemyBase>(BaseChar);
				HandSocket->AttachActor(EnChar->EquippedEnemyRangedWeapon, EnChar->GetMesh());
				Super::Equip(EnChar->GetMesh(), FName("RangedSocket"), NewOwner, NewInstigator);
			}
		}
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

void ARangedWeapon::SetRangedWeaponInformationText(UWidgetComponent* RangedItemInfoComp, ARangedWeapon* RangedWeapon)
{
	if (!IsValid(RangedItemInfoComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetRangedWeaponInformationText: ItemInfoComp is invalid."));
		return;
	}

	// Ensure an instance exists (especially after hot reload/PIE restarts)
	if (!RangedItemInfoComp->GetUserWidgetObject())
	{
		RangedItemInfoComp->InitWidget();
	}

	UUserWidget* RawWidget = RangedItemInfoComp->GetUserWidgetObject();
	if (!IsValid(RawWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetRangedWeaponInformationText: No widget instance on %s (class: %s)."),
			*GetNameSafe(RangedItemInfoComp),
			*GetNameSafe(RangedItemInfoComp->GetWidgetClass()));
		return;
	}

	// Cast to the exact widget you expect
	if (UItemInfoWidgetBase* Info = Cast<UItemInfoWidgetBase>(RawWidget))
	{
		// Safely update fields; example properties—adjust to yours
		Info->SetRangedInfo(RangedWeaponName, RangedWeaponDescription, RangedWeaponType, RangedWeaponRarity, RangedWeaponDamage); 
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetRangedWeaponInformationText: Expected URangedWeaponInfoWidget, got %s on %s."),
			*GetNameSafe(RawWidget->GetClass()),
			*GetNameSafe(RangedItemInfoComp));
	}
}
	