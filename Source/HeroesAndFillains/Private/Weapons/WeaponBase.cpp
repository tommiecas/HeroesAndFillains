// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"

#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "HUD/PickupGearWidget.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Components/SceneComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/AmmoPickUp.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/PickupGearWidget.h"
#include "NiagaraComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "HUD/Widgets/HAFUserWidget.h"
#include "Weapons/Melee/StormWeapons.h"

AWeaponBase::AWeaponBase()
	: Super() 
{
	PrimaryActorTick.bCanEverTick = false;
	// ✅ Then create and attach the mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Reset transforms
	WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
	WeaponMesh->SetRelativeScale3D(InitialMeshScale);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	WeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	// EnableCustomDepth(true); // Enable custom depth rendering for the mesh

	check(AreaSphere);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionObjectType(ECC_Pickupable);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	PickupGearWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponent"));
	PickupGearWidgetComponent->SetupAttachment(WeaponMesh);
	PickupGearWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupGearWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupGearWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PickupGearWidgetComponent->SetGenerateOverlapEvents(true);
	PickupGearWidgetComponent->SetVisibility(false);
	PickupGearWidgetComponent->SetWidgetClass(PickupGearWidgetClass);
	PickupGearWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponent->SetDrawSize(FVector2D(300.f, 50.f));
	PickupGearWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupGearWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	PickupGearWidgetComponent->SetDrawAtDesiredSize(true);
	PickupGearWidgetComponent->SetTickWhenOffscreen(true);
	PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
	PickupGearWidgetComponent->TranslucencySortPriority = 5;
	PickupGearWidgetComponent->SetVisibility(false);

	PickupGearWidget = CreateDefaultSubobject<UUserWidget>(TEXT("PickupGearWidget"));
	PickupGearWidget = PickupGearWidgetComponent->GetUserWidgetObject();
	
	ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
	ItemInfoWidgetComponent->SetupAttachment(WeaponMesh);
	ItemInfoWidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemInfoWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemInfoWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ItemInfoWidgetComponent->SetGenerateOverlapEvents(true);
	ItemInfoWidgetComponent->SetVisibility(false);
	ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
	ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
	ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // above the weapon
	ItemInfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
	ItemInfoWidgetComponent->SetTickWhenOffscreen(true);
	ItemInfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
	ItemInfoWidgetComponent->TranslucencySortPriority = 5;
	ItemInfoWidgetComponent->SetVisibility(false);

	ItemInfoWidget = CreateDefaultSubobject<UUserWidget>(TEXT("ItemInfoWidget"));
	ItemInfoWidget = ItemInfoWidgetComponent->GetUserWidgetObject();

	HoverLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoverLight"));
	HoverLight->SetupAttachment(WeaponMesh);

	// Settings
	HoverLight->SetIntensity(2000.f);  // How bright
	HoverLight->SetAttenuationRadius(300.f); // How far it shines
	HoverLight->SetLightColor(FLinearColor(0.f, 0.5f, 1.f)); // Slight blue glow
	HoverLight->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Glow slightly under rifle
	HoverLight->bUseInverseSquaredFalloff = false; // Makes intensity easier to control
	HoverLight->SetVisibility(true);

	HoverDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("HoverDecal"));
	HoverDecal->SetupAttachment(WeaponMesh);
	HoverDecal->DecalSize = FVector(64.f, 128.f, 128.f); // Flat and wide
	HoverDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // Face it downward
	HoverDecal->SetRelativeLocation(FVector(0.f, 0.f, -55.f)); // Slightly under rifle
	HoverDecal->SetVisibility(true);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PickupGearWidgetComponent && ItemInfoWidgetComponent)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->PlayerCameraManager->GetCameraLocation());
			PickupGearWidgetComponent->SetWorldRotation(LookRotation);
			ItemInfoWidgetComponent->SetWorldRotation(LookRotation);
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

void AWeaponBase::ShowPickupAndInfoWidgets(bool bShow)
{
	Super::ShowPickupAndInfoWidgets(bShow);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponBase, WeaponState);
	DOREPLIFETIME(AWeaponBase, WeaponCategory);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (AStormWeapons* StormWeapon = Cast<AStormWeapons>(this)) return;
	
	if (PickupGearWidgetComponent)
	{
		PickupGearWidgetComponent->UpdateComponentToWorld();
	}
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->UpdateComponentToWorld();
	}
	if (!PickupGearWidgetComponent)
	{
		PickupGearWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupGearWidgetComponent"));
		if (PickupGearWidgetComponent)
		{
			PickupGearWidgetComponent->RegisterComponent();
			PickupGearWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupGearWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupGearWidgetComponent->InitWidget();
			PickupGearWidgetComponent->SetVisibility(true);
			PickupGearWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupGearWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupGearWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			PickupGearWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			PickupGearWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			PickupGearWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			PickupGearWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			PickupGearWidgetComponent->SetDrawAtDesiredSize(true);
			PickupGearWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			PickupGearWidgetComponent->SetTickWhenOffscreen(true);
			PickupGearWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupGearWidgetComponent->TranslucencySortPriority = 5;
			if (auto* W = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject()))
			{
				W->PickupGearOwningComponent = PickupGearWidgetComponent;
			}
		}
	}
	if (!ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		if (ItemInfoWidgetComponent)
		{
			ItemInfoWidgetComponent->RegisterComponent();
			ItemInfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ItemInfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			ItemInfoWidgetComponent->InitWidget();
			ItemInfoWidgetComponent->SetVisibility(true);
			PickupGearWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupGearWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupGearWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			ItemInfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			ItemInfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
			ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
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
	if (ItemInfoWidgetComponent)
	{
		if (AActor* PickupItemActor = Cast<AActor>(ItemInfoWidgetComponent->GetOwner()))
		{
			if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(PickupItemActor)) MeleeWeapon->SetMeleeWeaponInformationText(ItemInfoWidgetComponent, MeleeWeapon);
			else if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(PickupItemActor)) RangedWeapon->SetRangedWeaponInformationText(ItemInfoWidgetComponent, RangedWeapon);
			else if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(PickupItemActor)) AmmoPickup->SetAmmoPickupInformationText(ItemInfoWidgetComponent, AmmoPickup);
			else return;
		}
	}
}

void AWeaponBase::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeaponBase::OnRep_WeaponCategory()
{
	SetEquippedWeaponState();
}

void AWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr)
	{
		FillainOwnerCharacter = nullptr;
		FillainOwnerController = nullptr;
	}
}

void AWeaponBase::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	// Set up attachment rules
	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale
		true);

	// Perform the attachment
	WeaponMesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
}

void AWeaponBase::SetHandsNeeded(AWeaponBase* WeaponBase)
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		HandsNeeded = EHandsNeeded::EHN_TwoHandedWeapon;
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword || WeaponType == EWeaponType:: EWT_SandSword || WeaponType == EWeaponType:: EWT_SoulSword || WeaponType == EWeaponType:: EWT_ShadowSword || WeaponType == EWeaponType:: EWT_SkyMace || WeaponType ==EWeaponType::EWT_MajixProjectile || WeaponType == EWeaponType::EWT_FireBolt)
	{
		HandsNeeded = EHandsNeeded::EHN_OneHandedWeapon;
	}
}

void AWeaponBase::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeaponBase::DisableSphereCollision()
{
	if (AreaSphere)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeaponBase::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeaponBase::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeaponBase::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
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
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AActor* OwnerCharacter = GetOwner(); // Typically set on equip
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // Ignore the weapon itself
	if (OwnerCharacter) TraceParams.AddIgnoredActor(OwnerCharacter); // ✅ Ignore the wielder!
	// UE_LOG(LogTemp, Warning, TEXT("Attaching %s to %s at socket %s"), *GetName(), *InParent->GetName(), *InSocketName.ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Post-Attach Location: %s"), *GetActorLocation().ToString());
}


void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ShowPickupAndInfoWidgets(true);
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex);
	ShowPickupAndInfoWidgets(false);
}

void AWeaponBase::SetEquippedWeaponState()
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

void AWeaponBase::OnWeaponStateSet()
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

void AWeaponBase::OnEquippedOneHanded()
{
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = false;
			bShouldFloatSpin = false;
			ShowPickupAndInfoWidgets(false);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnableCustomDepth(false);
	
			if (HoverDecal) HoverDecal->SetVisibility(false);
			if (HoverLight) HoverLight->SetVisibility(false);
		},
		0.1f,
		false
	);
	// Safely destroy existing widgets
	if (PickupGearWidgetComponent) { PickupGearWidgetComponent->DestroyComponent(); PickupGearWidgetComponent = nullptr; }
	if (ItemInfoWidgetComponent) { ItemInfoWidgetComponent->DestroyComponent(); ItemInfoWidgetComponent = nullptr; }
}

void AWeaponBase::OnEquippedTwoHanded()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = false;
			bShouldFloatSpin = false;
			ShowPickupAndInfoWidgets(false);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnableCustomDepth(false);
	
			if (HoverDecal) HoverDecal->SetVisibility(false);
			if (HoverLight) HoverLight->SetVisibility(false);
		},
		0.1f,
		false
	);
	// Safely destroy existing widgets
	if (PickupGearWidgetComponent) { PickupGearWidgetComponent->DestroyComponent(); PickupGearWidgetComponent = nullptr; }
	if (ItemInfoWidgetComponent) { ItemInfoWidgetComponent->DestroyComponent(); ItemInfoWidgetComponent = nullptr; }
}

void AWeaponBase::OnDropped()
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
		}
		UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupWidgetA)
		{
			PickupGearWidgetComponent = PickupWidgetA->GetOwningWidgetComponent();
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
		}
		UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
		if (InfoWidget)
		{
			ItemInfoWidgetComponent = InfoWidget->GetItemInfoOwningComponent();
		}
	}
}

void AWeaponBase::OnEquippedSecondary()
{
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->DestroyComponent(); AreaSphere = nullptr;
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
	HoverDecal->SetVisibility(false);
	HoverLight->SetVisibility(false);

	// Safely destroy existing widgets
	if (PickupGearWidgetComponent) { PickupGearWidgetComponent->DestroyComponent(); PickupGearWidgetComponent = nullptr; }
	if (ItemInfoWidgetComponent) { ItemInfoWidgetComponent->DestroyComponent(); ItemInfoWidgetComponent = nullptr; }
}

void AWeaponBase::SetOneOrTwoHandedWeapon(AWeaponBase* Weapon)
{
	if (!Weapon) return;
    
	// Validate the weapon's current state
	if (Weapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded ||
		Weapon->WeaponState == EWeaponState::EWS_EquippedOneHanded)
	{
		// Handle already equipped weapon
		return;
	}

	if (Weapon->WeaponType == EWeaponType::EWT_RocketLauncher || 
		Weapon->WeaponType == EWeaponType::EWT_GrenadeLauncher || 
		Weapon->WeaponType == EWeaponType::EWT_SniperRifle || 
		Weapon->WeaponType == EWeaponType::EWT_Shotgun || 
		Weapon->WeaponType == EWeaponType::EWT_ChaosSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		TwoHandedWeapon = Weapon;  // Store the weapon reference correctly
	}
	else if (Weapon->WeaponType == EWeaponType::EWT_AssaultRifle || 
			 Weapon->WeaponType == EWeaponType::EWT_SubmachineGun || 
			 Weapon->WeaponType == EWeaponType::EWT_Pistol || 
			 Weapon->WeaponType == EWeaponType::EWT_RubySword || 
			 Weapon->WeaponType == EWeaponType::EWT_SapphireSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
		OneHandedWeapon = Weapon;  // Store the weapon reference correctly
	}
}

void AWeaponBase::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
}

void AWeaponBase::WeaponDropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	FillainOwnerCharacter = nullptr;
	FillainOwnerController = nullptr;
}