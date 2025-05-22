// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/WeaponBase.h"

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
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/RangedInfoWidgetComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh")); 
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	WeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	EnableCustomDepth(true); // Enable custom depth rendering for the mesh
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	
	PickupGearWidgetComponentA = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetA"));
	PickupGearWidgetComponentA->SetupAttachment(RootComponent);
	PickupGearWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponentA->SetDrawSize(FVector2D(200.f, 50.f));
	PickupGearWidgetComponentA->SetVisibility(false, true);
	if (UPickupGearWidget* PickupGearWidget = Cast<UPickupGearWidget>(PickupGearWidgetComponentA->GetUserWidgetObject()))
	{
		PickupGearWidget->OwningActor = this; // assuming this is AWeaponBase or derived
	}
	
	PickupGearWidgetComponentB = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetB"));
	PickupGearWidgetComponentB->SetupAttachment(RootComponent);
	PickupGearWidgetComponentB->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponentB->SetDrawSize(FVector2D(200.f, 50.f));
	PickupGearWidgetComponentB->SetVisibility(false, true);
	if (UPickupGearWidget* PickupGearWidget = Cast<UPickupGearWidget>(PickupGearWidgetComponentB->GetUserWidgetObject()))
	{
		PickupGearWidget->OwningActor = this; // assuming this is AWeaponBase or derived
	}
	
	ItemInfoWidgetComponentA = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetA"));
	ItemInfoWidgetComponentA->SetupAttachment(RootComponent);
	ItemInfoWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentA->SetDrawSize(FVector2D(300.f, 100.f));
	ItemInfoWidgetComponentA->SetVisibility(false, true); // ← true = Propagate to editor
	bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));

	ItemInfoWidgetComponentB = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetB"));
	ItemInfoWidgetComponentB->SetupAttachment(RootComponent);
	ItemInfoWidgetComponentB->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentB->SetDrawSize(FVector2D(300.f, 100.f));
	ItemInfoWidgetComponentB->SetVisibility(false, true); // ← true = Propagate to editor
	bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
	
	HoverLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoverLight"));
	HoverLight->SetupAttachment(RootComponent);

	// Settings
	HoverLight->SetIntensity(2000.f);  // How bright
	HoverLight->SetAttenuationRadius(300.f); // How far it shines
	HoverLight->SetLightColor(FLinearColor(0.f, 0.5f, 1.f)); // Slight blue glow
	HoverLight->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Glow slightly under rifle
	HoverLight->bUseInverseSquaredFalloff = false; // Makes intensity easier to control
	HoverLight->SetVisibility(true);

	HoverDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("HoverDecal"));
	HoverDecal->SetupAttachment(RootComponent);

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
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

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
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponBase, WeaponState);
}

void AWeaponBase::ShowPickupAndInfoWidgets(bool bShow)
{
	if (PickupGearWidgetComponentA) PickupGearWidgetComponentA->SetVisibility(bShow);
	if (PickupGearWidgetComponentB) PickupGearWidgetComponentB->SetVisibility(bShow);
	if (ItemInfoWidgetComponentA) ItemInfoWidgetComponentA->SetVisibility(bShow);
	if (ItemInfoWidgetComponentB) ItemInfoWidgetComponentB->SetVisibility(bShow);
}

void AWeaponBase::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// Use AddUniqueDynamic to safely bind
	AreaSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWeaponBase::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AWeaponBase::OnSphereEndOverlap);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	ShowPickupAndInfoWidgets(false);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in WeaponBaseBeginPlay"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in WeaponBaseBeginPlay"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
}


void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingWeapon(this);
	}
	bShouldFloatSpin = false;
	AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
	bShouldHover = false;

	ShowPickupAndInfoWidgets(true);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in WeaponBase Overlap"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in WeaponBaseOverlap"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
	
	UE_LOG(LogTemp, Log, TEXT("%s overlapped with %s"),
		*GetName(), *GetNameSafe(OtherActor));
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingWeapon(nullptr);
	}

	ShowPickupAndInfoWidgets(false);
	
	UE_LOG(LogTemp, Log, TEXT("%s ended overlap with %s"),
		*GetName(), *GetNameSafe(OtherActor));
}

float AWeaponBase::TransformedSin() const
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AWeaponBase::TransformedCos() const
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
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

void AWeaponBase::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
}

void AWeaponBase::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;
	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}
}

void AWeaponBase::OnEquipped()
{
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s in WeaponBaseEquipped"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s in WeaponBaseEquipped"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->DestroyComponent(); AreaSphere = nullptr;
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
	HoverDecal->SetVisibility(false);
	HoverLight->SetVisibility(false);

	// Safely destroy existing widgets
	if (PickupGearWidgetComponentA) { PickupGearWidgetComponentA->DestroyComponent(); PickupGearWidgetComponentA = nullptr; }
	if (PickupGearWidgetComponentB) { PickupGearWidgetComponentB->DestroyComponent(); PickupGearWidgetComponentB = nullptr; }
	if (ItemInfoWidgetComponentA) { ItemInfoWidgetComponentA->DestroyComponent(); ItemInfoWidgetComponentA = nullptr; }
	if (ItemInfoWidgetComponentB) { ItemInfoWidgetComponentB->DestroyComponent(); ItemInfoWidgetComponentB = nullptr; }
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
		AreaSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//AreaSphere->SetupAttachment(GetRootComponent());
		//AreaSphere->SetupAttachment(RootComponent);
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

	if (!PickupGearWidgetComponentA)
	{
		PickupGearWidgetComponentA = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		PickupGearWidgetComponentA->RegisterComponent();
		PickupGearWidgetComponentA->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PickupGearWidgetComponentA->SetWidgetClass(UUserWidget::StaticClass());
		PickupGearWidgetComponentA->InitWidget();
		PickupGearWidgetComponentA->SetVisibility(true);
	}

	if (!PickupGearWidgetComponentB)
	{
		PickupGearWidgetComponentB = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentB"));
		PickupGearWidgetComponentB->RegisterComponent();
		PickupGearWidgetComponentB->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PickupGearWidgetComponentB->SetWidgetClass(UUserWidget::StaticClass());
		PickupGearWidgetComponentB->InitWidget();
		PickupGearWidgetComponentB->SetVisibility(true);
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
	if (PickupGearWidgetComponentA) { PickupGearWidgetComponentA->DestroyComponent(); PickupGearWidgetComponentA = nullptr; }
	if (PickupGearWidgetComponentB) { PickupGearWidgetComponentB->DestroyComponent(); PickupGearWidgetComponentB = nullptr; }
	if (ItemInfoWidgetComponentA) { ItemInfoWidgetComponentA->DestroyComponent(); ItemInfoWidgetComponentA = nullptr; }
	if (ItemInfoWidgetComponentB) { ItemInfoWidgetComponentB->DestroyComponent(); ItemInfoWidgetComponentB = nullptr; }
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

