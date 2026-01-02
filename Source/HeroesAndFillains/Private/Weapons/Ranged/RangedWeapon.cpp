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
	PrimaryActorTick.bCanEverTick = false;
	// ✅ Then create and attach the mesh
	RangedWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(RangedWeaponMesh);
	
	RangedWeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RangedWeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Reset transforms
	RangedWeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	RangedWeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
	RangedWeaponMesh->SetRelativeScale3D(InitialMeshScale);
	RangedWeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	RangedWeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	// EnableCustomDepth(true); // Enable custom depth rendering for the mesh

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereCollision->SetupAttachment(RangedWeaponMesh);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_Pickupable);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponent"));
	PickupWidgetComponent->SetupAttachment(RangedWeaponMesh);
	PickupWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PickupWidgetComponent->SetGenerateOverlapEvents(true);
	PickupWidgetComponent->SetVisibility(false);
	PickupWidgetComponent->SetWidgetClass(PickupWidgetClass);
	PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	PickupWidgetComponent->SetDrawSize(FVector2D(300.f, 50.f));
	PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	PickupWidgetComponent->SetDrawAtDesiredSize(true);
	PickupWidgetComponent->SetTickWhenOffscreen(true);
	PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
	PickupWidgetComponent->TranslucencySortPriority = 5;
	PickupWidgetComponent->SetVisibility(false);
	
	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
	InfoWidgetComponent->SetupAttachment(RangedWeaponMesh);
	InfoWidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InfoWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	InfoWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InfoWidgetComponent->SetGenerateOverlapEvents(true);
	InfoWidgetComponent->SetVisibility(false);
	InfoWidgetComponent->SetWidgetClass(InfoWidgetClass);
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
	InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // above the weapon
	InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	InfoWidgetComponent->SetDrawAtDesiredSize(true);
	InfoWidgetComponent->SetTickWhenOffscreen(true);
	InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
	InfoWidgetComponent->TranslucencySortPriority = 5;
	InfoWidgetComponent->SetVisibility(false);

	HoveringLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoveringLight"));
	HoveringLight->SetupAttachment(RangedWeaponMesh);

	// Settings
	HoveringLight->SetIntensity(2000.f);  // How bright
	HoveringLight->SetAttenuationRadius(300.f); // How far it shines
	HoveringLight->SetLightColor(FLinearColor(0.f, 0.5f, 1.f)); // Slight blue glow
	HoveringLight->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Glow slightly under rifle
	HoveringLight->bUseInverseSquaredFalloff = false; // Makes intensity easier to control
	HoveringLight->SetVisibility(true);

	HoveringDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("HoveringDecal"));
	HoveringDecal->SetupAttachment(RangedWeaponMesh);
	HoveringDecal->DecalSize = FVector(64.f, 128.f, 128.f); // Flat and wide
	HoveringDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // Face it downward
	HoveringDecal->SetRelativeLocation(FVector(0.f, 0.f, -55.f)); // Slightly under rifle
	HoveringDecal->SetVisibility(true);
	
	// Initialize the WeaponAmmoMap
	RangedWeaponAmmoMap.Add(ERangedType::ERT_AssaultRifle,    EAmmoType::EAT_ARAmmo);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_RocketLauncher,  EAmmoType::EAT_Rockets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Pistol,          EAmmoType::EAT_Bullets);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SubmachineGun,   EAmmoType::EAT_Magazines);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_Shotgun,         EAmmoType::EAT_Shells);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_GrenadeLauncher, EAmmoType::EAT_LaunchedGrenades);
	RangedWeaponAmmoMap.Add(ERangedType::ERT_SniperRifle,     EAmmoType::EAT_SniperAmmo);

	if (IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);        // or World
		InfoWidgetComponent->SetDrawAtDesiredSize(true);
		if (InfoWidgetClass)
		{
			InfoWidgetComponent->SetWidgetClass(InfoWidgetClass);
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

void ARangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidgetComponent)
	{
		PickupWidgetComponent->UpdateComponentToWorld();
	}
	if (InfoWidgetComponent)
	{
		InfoWidgetComponent->UpdateComponentToWorld();
	}
	if (!PickupWidgetComponent)
	{
		PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupGearWidgetComponent"));
		if (PickupWidgetComponent)
		{
			PickupWidgetComponent->RegisterComponent();
			PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupWidgetComponent->InitWidget();
			PickupWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			PickupWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			PickupWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			PickupWidgetComponent->SetDrawAtDesiredSize(true);
			PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			PickupWidgetComponent->SetTickWhenOffscreen(true);
			PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupWidgetComponent->TranslucencySortPriority = 5;
			if (auto* W = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject()))
			{
				W->PickupGearOwningComponent = PickupWidgetComponent;
			}
		}
	}
	if (!InfoWidgetComponent)
	{
		InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		if (InfoWidgetComponent)
		{
			InfoWidgetComponent->RegisterComponent();
			InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			InfoWidgetComponent->InitWidget();
			InfoWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			InfoWidgetComponent->SetDrawAtDesiredSize(true);
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			InfoWidgetComponent->SetTickWhenOffscreen(true);
			InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			InfoWidgetComponent->TranslucencySortPriority = 5;
			if (auto* IIW = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject()))
			{
				IIW->ItemInfoOwningComponent = InfoWidgetComponent;
			}
		}
	}
	if (InfoWidgetComponent)
	{
		if (AActor* PickupItemActor = Cast<AActor>(InfoWidgetComponent->GetOwner()))
		{
			if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(PickupItemActor)) MeleeWeapon->SetMeleeWeaponInformationText(InfoWidgetComponent, MeleeWeapon);
			else if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(PickupItemActor)) RangedWeapon->SetRangedWeaponInformationText(InfoWidgetComponent, RangedWeapon);
			else if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(PickupItemActor)) AmmoPickup->SetAmmoPickupInformationText(InfoWidgetComponent, AmmoPickup);
			else return;
		}
	}
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
	if (WeaponState == EWeaponState::EWS_EquippedOneHanded) HighPingOnEquippedOneHanded();
	if (WeaponState == EWeaponState::EWS_EquippedTwoHanded) HighPingOnEquippedTwoHanded();
	if (WeaponState == EWeaponState::EWS_EquippedSecondary) HighPingOnEquippedSecondary();
	if (WeaponState == EWeaponState::EWS_Dropped) HighPingOnDropped();;
	if (WeaponState == EWeaponState::EWS_Unclaimed) return;

	SetRangedWeaponInformationText(GetInfoWidgetComponent(), this);
}

void ARangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PickupWidgetComponent && InfoWidgetComponent)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->PlayerCameraManager->GetCameraLocation());
			PickupWidgetComponent->SetWorldRotation(LookRotation);
			InfoWidgetComponent->SetWorldRotation(LookRotation);
		}
	}
	
	if (bShouldHover)
	{
		float RunTime = GetGameTimeSinceCreation(); // How long this actor has existed
		float DeltaHeight = FMath::Sin(RunTime * HoverSpeed) * HoverAmplitude * DeltaTime;

		FVector NewLocation = GetActorLocation();
		NewLocation.Z += DeltaHeight;
		SetActorLocation(NewLocation);
	}

	if (bShouldFloatSpin) // You can make a bool for it
	{
		AddActorLocalRotation(FRotator(0.f, 30.f * DeltaTime, 0.f)); // 30 degrees per second
	}
	
	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
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
	RangedWeaponMesh->SetSimulatePhysics(true);
	RangedWeaponMesh->SetEnableGravity(true);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (!SphereCollision)
	{
		SphereCollision = NewObject<USphereComponent>(this, TEXT("Area Sphere"));
		SphereCollision->RegisterComponent();
		SphereCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	if (HasAuthority())
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = true;
			bShouldFloatSpin = true;
			RangedWeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
			RangedWeaponMesh->MarkRenderStateDirty();
			EnableCustomDepth(true);
	
			HoveringDecal->SetVisibility(true);
			HoveringLight->SetVisibility(true);
		},
		0.1f,
		false);
	
	if (!IsValid(this)) return;
    
	if (!PickupWidgetComponent && !IsValid(PickupWidgetComponent))
	{
		PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		if (PickupWidgetComponent)
		{
			PickupWidgetComponent->RegisterComponent();
			PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupWidgetComponent->InitWidget();
			PickupWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetTickWhenOffscreen(true);
			PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupWidgetComponent->TranslucencySortPriority = 5;
			if (auto* W = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject()))
			{
				W->PickupGearOwningComponent = PickupWidgetComponent;
			}
		}
		UPickupGearWidget* PUGWidget = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject());
		if (PUGWidget)
		{
			PickupWidgetComponent = PUGWidget->GetOwningWidgetComponent();
		}
	}
	
	if (!IsValid(this)) return;
    
	if (!InfoWidgetComponent && !IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		if (InfoWidgetComponent)
		{
			InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			InfoWidgetComponent->InitWidget();
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			InfoWidgetComponent->SetDrawAtDesiredSize(true);
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetTickWhenOffscreen(true);
			InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			InfoWidgetComponent->TranslucencySortPriority = 5;
			if (auto* IIW = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject()))
			{
				IIW->ItemInfoOwningComponent = InfoWidgetComponent;
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
	if (!IsValid(RangedWeaponMesh)) return; // More robust check

	if (!IsValid(GetWorld())) return; // Add world validity check
    
	// UE_LOG(LogTemp, Warning, TEXT("ARangedWeapon::Fire called on %s"), *GetName());

	if (FireAnimation && IsValid(RangedWeaponMesh))
	{
		RangedWeaponMesh->PlayAnimation(FireAnimation, false);
	}

	if (CasingClass && IsValid(RangedWeaponMesh))
	{
		const USkeletalMeshSocket* AmmoEjectSocket = RangedWeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(RangedWeaponMesh);
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
			}
		}
	}
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	RangedWeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
	ItemState = EItemState::EIS_Equipped;
	bIsEquipped = true;
	SetOwner(NewOwner);
	ShowPickupAndInfoWidgets(false);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	PlayEquipSound();
	DisableSphereCollision();
	SetHandsNeeded(this);
	SetEquippedWeaponState();
	if (HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon) OnEquippedOneHanded();
	if (HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon) OnEquippedTwoHanded();
	SetOneOrTwoHandedWeapon(this);
	DeactivateEmbers();
	bShouldHover = false;
	bShouldFloatSpin = false;
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AActor* OwnerCharacter = GetOwner(); // Typically set on equip
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // Ignore the weapon itself
	if (OwnerCharacter) TraceParams.AddIgnoredActor(OwnerCharacter); // ✅ Ignore the wielder!
	// UE_LOG(LogTemp, Warning, TEXT("Attaching %s to %s at socket %s"), *GetName(), *InParent->GetName(), *InSocketName.ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Post-Attach Location: %s"), *GetActorLocation().ToString());
}

void ARangedWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	// Set up attachment rules
	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale
		true);

	// Perform the attachment
	RangedWeaponMesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
}

bool ARangedWeapon::IsRangedWeaponEmpty()
{
	return (Ammo <= 0);
}

bool ARangedWeapon::IsRangedWeaponFull()
{
	return Ammo == MagCapacity;
}

void ARangedWeapon::DisableSphereCollision()
{
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ARangedWeapon::EnableCustomDepth(bool bEnable)
{
	if (RangedWeaponMesh)
	{
		RangedWeaponMesh->SetRenderCustomDepth(bEnable);
	}
	if (RangedWeaponMesh)
	{
		RangedWeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

FVector ARangedWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetRangedWeaponMesh()->GetSocketByName("MuzzleFlashSocket");
	if (MuzzleFlashSocket == nullptr) return FVector();
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetRangedWeaponMesh());
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

void ARangedWeapon::OnEquippedOneHanded()
{
	RangedWeaponMesh->SetSimulatePhysics(false);
	RangedWeaponMesh->SetEnableGravity(false);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = false;
			bShouldFloatSpin = false;
			ShowPickupAndInfoWidgets(false);
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnableCustomDepth(false);
	
			if (HoveringDecal) HoveringDecal->SetVisibility(false);
			if (HoveringLight) HoveringLight->SetVisibility(false);
		},
		0.1f,
		false
	);
	// Safely destroy existing widgets
	if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
	if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
}

void ARangedWeapon::OnEquippedTwoHanded()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangedWeaponMesh->SetSimulatePhysics(false);
	RangedWeaponMesh->SetEnableGravity(false);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = false;
			bShouldFloatSpin = false;
			ShowPickupAndInfoWidgets(false);
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnableCustomDepth(false);
	
			if (HoveringDecal) HoveringDecal->SetVisibility(false);
			if (HoveringLight) HoveringLight->SetVisibility(false);
		},
		0.1f,
		false
	);
	// Safely destroy existing widgets
	if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
	if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
}

void ARangedWeapon::OnDropped()
{
	RangedWeaponMesh->SetSimulatePhysics(true);
	RangedWeaponMesh->SetEnableGravity(true);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (!SphereCollision)
	{
		SphereCollision = NewObject<USphereComponent>(this, TEXT("Area Sphere"));
		SphereCollision->RegisterComponent();
		SphereCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	if (HasAuthority())
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
	RangedWeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = true;
			bShouldFloatSpin = true;
			RangedWeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
			RangedWeaponMesh->MarkRenderStateDirty();
			EnableCustomDepth(true);
	
			HoveringDecal->SetVisibility(true);
			HoveringLight->SetVisibility(true);
		},
		0.1f,
		false);
	
	if (!IsValid(this)) return;
    
	if (!PickupWidgetComponent && !IsValid(PickupWidgetComponent))
	{
		PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		if (PickupWidgetComponent)
		{
			PickupWidgetComponent->RegisterComponent();
			PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupWidgetComponent->InitWidget();
			PickupWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetTickWhenOffscreen(true);
			PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupWidgetComponent->TranslucencySortPriority = 5;
		}
		UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject());
		if (PickupWidgetA)
		{
			PickupWidgetComponent = PickupWidgetA->GetOwningWidgetComponent();
		}
	}
	if (!IsValid(this)) return;
    
	if (!InfoWidgetComponent && !IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		if (InfoWidgetComponent)
		{
			InfoWidgetComponent->RegisterComponent();
			InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			InfoWidgetComponent->InitWidget();
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			InfoWidgetComponent->SetDrawAtDesiredSize(true);
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetTickWhenOffscreen(true);
			InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			InfoWidgetComponent->TranslucencySortPriority = 5;
		}
		UItemInfoWidgetBase* IWidget = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject());
		if (IWidget)
		{
			InfoWidgetComponent = IWidget->GetItemInfoOwningComponent();
		}
	}
}

void ARangedWeapon::OnEquippedSecondary()
{
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->DestroyComponent(); SphereCollision = nullptr;
	RangedWeaponMesh->SetSimulatePhysics(false);
	RangedWeaponMesh->SetEnableGravity(false);
	RangedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
	HoveringDecal->SetVisibility(false);
	HoveringLight->SetVisibility(false);

	// Safely destroy existing widgets
	if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
	if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
}

void ARangedWeapon::WeaponDropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	RangedWeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	FillainOwnerCharacter = nullptr;
	FillainOwnerController = nullptr;
}

void ARangedWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_EquippedOneHanded:
		OnEquippedOneHanded();
		break;
	case EWeaponState::EWS_EquippedTwoHanded:
		OnEquippedTwoHanded();
		break;
	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}
}

void ARangedWeapon::SetEquippedWeaponState()
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		OnWeaponStateSet();
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword || WeaponType == EWeaponType:: EWT_SandSword || WeaponType == EWeaponType:: EWT_SoulSword || WeaponType == EWeaponType:: EWT_ShadowSword || WeaponType == EWeaponType:: EWT_SkyMace)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
		OnWeaponStateSet();
	}
}

void ARangedWeapon::SetEquippedWeaponCategory()
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun)
	{
		WeaponCategory = EWeaponCategory::EWC_TwoHandedFirearm;
		OnWeaponCategorySet();
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol)
	{
		WeaponCategory = EWeaponCategory::EWC_OneHandedFirearm;
		OnWeaponCategorySet();
	}
	if (WeaponType == EWeaponType::EWT_ChaosSword)
	{
		WeaponCategory = EWeaponCategory::EWC_TwoHandedSword;
		OnWeaponCategorySet();
	}
	if (WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType::EWT_SapphireSword || WeaponType == EWeaponType::EWT_SandSword || WeaponType == EWeaponType::EWT_ShadowSword || WeaponType == EWeaponType::EWT_SoulSword || WeaponType == EWeaponType::EWT_SkyMace)
	{
		WeaponCategory = EWeaponCategory::EWC_OneHandedSword;
		OnWeaponCategorySet();
	}
}

void ARangedWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void ARangedWeapon::OnRep_WeaponCategory()
{
	OnWeaponCategorySet();
}

void ARangedWeapon::OnWeaponCategorySet()
{
	switch (WeaponCategory)
	{
	case EWeaponCategory::EWC_Firearm:
		break;
	case EWeaponCategory::EWC_OneHandedFirearm:
		OnEquippedOneHanded();
		break;
	case EWeaponCategory::EWC_TwoHandedFirearm:
		OnEquippedTwoHanded();
		break;
	case EWeaponCategory::EWC_Launcher:
		OnEquippedTwoHanded();
		break;
	case EWeaponCategory::EWC_MajixSpell:
		break;
	case EWeaponCategory::EWC_NothingButYourFists:
		break;
	case EWeaponCategory::EWC_OneHandedSword:
		OnEquippedOneHanded();
		break;
	case EWeaponCategory::EWC_TwoHandedSword:
		OnEquippedTwoHanded();
		break;
	case EWeaponCategory::EWC_Sword:
		break;
	case EWeaponCategory::EWC_MAX:
		break;
	}
}