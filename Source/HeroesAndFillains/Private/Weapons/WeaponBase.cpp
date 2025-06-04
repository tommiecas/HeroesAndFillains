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
#include "Weapons/WeaponTypes.h"
#include "Components/SceneComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/AmmoPickUp.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/PickupGearWidget.h"

AWeaponBase::AWeaponBase()
	: Super() 
{
	PrimaryActorTick.bCanEverTick = false;

	// ✅ Then create and attach the mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	// Reset transforms
	WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
	WeaponMesh->SetRelativeScale3D(FVector(1.0f));
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	WeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	EnableCustomDepth(true); // Enable custom depth rendering for the mesh

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(Root);
	AreaSphere->SetSphereRadius(200.f);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);

	PickupGearWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponent"));
	PickupGearWidgetComponent->SetupAttachment(Root);
	PickupGearWidgetComponent->SetVisibility(false);
	PickupGearWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponent->SetDrawSize(FVector2D(300.f, 50.f));
	PickupGearWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupGearWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	PickupGearWidgetComponent->SetDrawAtDesiredSize(true);
	PickupGearWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	PickupGearWidgetComponent->SetVisibility(true);
	PickupGearWidgetComponent->SetTickWhenOffscreen(true);
	PickupGearWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
	PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
	PickupGearWidgetComponent->TranslucencySortPriority = 5;
	
	ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
	ItemInfoWidgetComponent->SetupAttachment(Root);
	ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
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
	
	HoverLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoverLight"));
	HoverLight->SetupAttachment(Root);

	// Settings
	HoverLight->SetIntensity(2000.f);  // How bright
	HoverLight->SetAttenuationRadius(300.f); // How far it shines
	HoverLight->SetLightColor(FLinearColor(0.f, 0.5f, 1.f)); // Slight blue glow
	HoverLight->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Glow slightly under rifle
	HoverLight->bUseInverseSquaredFalloff = false; // Makes intensity easier to control
	HoverLight->SetVisibility(true);

	HoverDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("HoverDecal"));
	HoverDecal->SetupAttachment(Root);

	// Settings 
	HoverDecal->DecalSize = FVector(64.f, 128.f, 128.f); // Flat and wide
	HoverDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // Face it downward
	HoverDecal->SetRelativeLocation(FVector(0.f, 0.f, -55.f)); // Slightly under rifle

	// Assign a material (you need a simple glowing decal material)
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMat(TEXT("Material'/Game/Materials/M_GlowDecal.M_GlowDecal'"));
	if (DecalMat.Succeeded())
	{
		HoverDecal->SetDecalMaterial(DecalMat.Object);
	}
	UE_LOG(LogTemp, Warning, TEXT("Mesh relative to root: %s"), *WeaponMesh->GetRelativeLocation().ToString());
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
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->UpdateComponentToWorld();
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: ItemInfoWidget world location: %s"),
			*ItemInfoWidgetComponent->GetComponentLocation().ToString());

		if (ItemInfoWidgetComponent->GetUserWidgetObject() == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("ERROR: ItemInfoWidgetComponent has no widget object!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget Class Loaded: %s"), *ItemInfoWidgetComponent->GetUserWidgetObject()->GetName());
		}
	}

	UPickupGearWidget* PickupWidget = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
	if (PickupWidget)
	{
		PickupWidget->OwningWidgetComponent = PickupGearWidgetComponent;
	}

	UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
	if (InfoWidget)
	{
		InfoWidget->OwningWidgetComponent = ItemInfoWidgetComponent;
	}
	
	ItemInfoWidgetComponent->SetVisibility(false);
	PickupGearWidgetComponent->SetVisibility(false);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
		if (AActor* PickupItemActor = Cast<AActor>(ItemInfoWidgetComponent->GetOwner()))
		{
			if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(PickupItemActor))
			{
				MeleeWeapon->SetMeleeWeaponInformationText(ItemInfoWidgetComponent, MeleeWeapon);
			}
			else if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(PickupItemActor))
			{
				RangedWeapon->SetRangedWeaponInformationText(ItemInfoWidgetComponent, RangedWeapon);
			}
			else if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(PickupItemActor))
			{
				AmmoPickup->SetAmmoPickupInformationText(ItemInfoWidgetComponent, AmmoPickup);
			}
		}
	}
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->UpdateComponentToWorld();
		UE_LOG(LogTemp, Warning, TEXT("Updated ItemInfoWidgetComponent world location: %s"),
			*ItemInfoWidgetComponent->GetComponentLocation().ToString());
	}
	ItemInfoWidgetComponent->SetVisibility(true);
	if (ItemInfoWidgetComponent && ItemInfoWidgetClass)
    {
        ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
    }

	UE_LOG(LogTemp, Warning, TEXT("Weapon spawned: %s"), *GetName());
	if (ItemInfoWidgetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("  WidgetClass: %s"),
			*GetNameSafe(ItemInfoWidgetComponent->GetWidgetClass()));
		UE_LOG(LogTemp, Warning, TEXT("  Widget World Location: %s"),
			*ItemInfoWidgetComponent->GetComponentLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("  Widget Visible: %s"),
			ItemInfoWidgetComponent->IsVisible() ? TEXT("YES") : TEXT("NO"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("  No WidgetComponent attached!"));
	}
}

void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || bIsEquipped) return;
	Super::OnSphereOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	bShouldFloatSpin = false;
	AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
	bShouldHover = false;

	if (!OtherActor || OtherActor == this) return;

	// Optional: Cast to your player character class for strict type check
	auto* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player)
		ShowPickupAndInfoWidgets(true);

	// Fade in Info Widget
	if (ItemInfoWidgetComponent)
	{
		UItemInfoWidgetBase* InfoInstanceA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
		if (InfoInstanceA && InfoInstanceA->FadeInAnimation)
		{
			InfoInstanceA->PlayAnimation(InfoInstanceA->FadeInAnimation);
		}
	}
	
	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponent)
	{
		UPickupGearWidget* PickupGearWidgetInstanceA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupGearWidgetInstanceA && PickupGearWidgetInstanceA->FadeInAnimation)
		{
			PickupGearWidgetInstanceA->PlayAnimation(PickupGearWidgetInstanceA->FadeInAnimation);
		}
	}
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if (!OtherActor || OtherActor == this || bIsEquipped) return;
	Super::OnSphereEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	if (!OtherActor || OtherActor == this) return;

	// Fade in Info Widget
	if (ItemInfoWidgetComponent)
	{
		UItemInfoWidgetBase* InfoInstancedWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
		if (InfoInstancedWidgetA && InfoInstancedWidgetA->FadeOutAnimation)
		{
			InfoInstancedWidgetA->PlayAnimation(InfoInstancedWidgetA->FadeOutAnimation);
		}
	}

	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponent)
	{
		UPickupGearWidget* PickupGearInstancedWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupGearInstancedWidgetA && PickupGearInstancedWidgetA->FadeOutAnimation)
		{
			PickupGearInstancedWidgetA->PlayAnimation(PickupGearInstancedWidgetA->FadeOutAnimation);
		}
	}	
	ShowPickupAndInfoWidgets(false);
}

void AWeaponBase::OnRep_WeaponState()
{
	OnWeaponStateSet();
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

void AWeaponBase::SetEquippedWeaponState()
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		OnWeaponStateSet();
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword)
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
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AWeaponBase::OnEquippedTwoHanded()
{
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AWeaponBase::OnDropped()
{
	bShouldHover = true;
	bShouldFloatSpin = true;
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (!AreaSphere)
	{
		AreaSphere = NewObject<USphereComponent>(this, TEXT("Area Sphere"));
		AreaSphere->RegisterComponent();
		AreaSphere->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
	
	HoverDecal->SetVisibility(true);
	HoverLight->SetVisibility(true);

	if (!PickupGearWidgetComponent)
	{
		PickupGearWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		PickupGearWidgetComponent->RegisterComponent();
		PickupGearWidgetComponent->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PickupGearWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
		PickupGearWidgetComponent->InitWidget();
		PickupGearWidgetComponent->SetVisibility(true);
		PickupGearWidgetComponent->SetTickWhenOffscreen(true);
		PickupGearWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
		PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
		PickupGearWidgetComponent->TranslucencySortPriority = 5;
		UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
		if (PickupWidgetA)
		{
			PickupWidgetA->OwningWidgetComponent = ItemInfoWidgetComponent;
		}
	}
	
	if (!ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponent"));
		ItemInfoWidgetComponent->RegisterComponent();
		ItemInfoWidgetComponent->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
		UItemInfoWidgetBase* InfoWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
		if (InfoWidgetA)
		{
			InfoWidgetA->OwningWidgetComponent = ItemInfoWidgetComponent;
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
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		Weapon = TwoHandedWeapon;
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword)
	{
		Weapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
		Weapon = OneHandedWeapon;
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

