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
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh")); 
	WeaponMesh->SetupAttachment(Root);
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
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	
	PickupGearWidgetComponentA = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponentA"));
	PickupGearWidgetComponentA->SetupAttachment(Root);
	PickupGearWidgetComponentA->SetVisibility(false);
	PickupGearWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponentA->SetDrawSize(FVector2D(300.f, 50.f));
	PickupGearWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupGearWidgetComponentA->SetPivot(FVector2D(0.5f, 0.5f));
	PickupGearWidgetComponentA->SetDrawAtDesiredSize(true);
	PickupGearWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	PickupGearWidgetComponentA->SetVisibility(true);
	PickupGearWidgetComponentA->SetTickWhenOffscreen(true);
	PickupGearWidgetComponentA->SetDrawSize(FVector2D(400.f, 200.f));
	PickupGearWidgetComponentA->SetWorldScale3D(FVector(1.0f));
	PickupGearWidgetComponentA->TranslucencySortPriority = 5;
	UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponentA->GetUserWidgetObject());
	if (PickupWidgetA)
	{
		PickupWidgetA->OwningWidgetComponent = ItemInfoWidgetComponentA;
	}
	
	ItemInfoWidgetComponentA = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponentA"));
	ItemInfoWidgetComponentA->SetupAttachment(Root);
	ItemInfoWidgetComponentA->SetWidgetClass(ItemInfoWidgetClass);
	ItemInfoWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentA->SetDrawSize(FVector2D(300.f, 200.f));
	ItemInfoWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
	ItemInfoWidgetComponentA->SetPivot(FVector2D(0.5f, 0.5f));
	ItemInfoWidgetComponentA->SetDrawAtDesiredSize(true);
	ItemInfoWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	ItemInfoWidgetComponentA->SetVisibility(true);
	ItemInfoWidgetComponentA->SetTickWhenOffscreen(true);
	ItemInfoWidgetComponentA->SetDrawSize(FVector2D(400.f, 200.f));
	ItemInfoWidgetComponentA->SetWorldScale3D(FVector(1.0f));
	ItemInfoWidgetComponentA->TranslucencySortPriority = 5;
	UItemInfoWidgetBase* InfoWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
	if (InfoWidgetA)
	{
		InfoWidgetA->OwningWidgetComponent = ItemInfoWidgetComponentA;
	}
	
	
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

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PickupGearWidgetComponentA && ItemInfoWidgetComponentA)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->PlayerCameraManager->GetCameraLocation());
			PickupGearWidgetComponentA->SetWorldRotation(LookRotation);
			ItemInfoWidgetComponentA->SetWorldRotation(LookRotation);
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
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponBase, WeaponState);
}

void AWeaponBase::ShowPickupAndInfoWidgets(bool bShow)
{
	if (PickupGearWidgetComponentA) PickupGearWidgetComponentA->SetVisibility(bShow);
	if (ItemInfoWidgetComponentA) ItemInfoWidgetComponentA->SetVisibility(bShow);
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
	
	ItemInfoWidgetComponentA->SetVisibility(false);
	PickupGearWidgetComponentA->SetVisibility(false);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
	// Use AddUniqueDynamic to safely bind
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnSphereEndOverlap);
	
	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
		ItemInfoWidgetInstanceA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (ItemInfoWidgetInstanceA)
		{
			if (AMeleeWeapon* MeleeWeaponA = Cast<AMeleeWeapon>(ItemInfoWidgetComponentA->GetOwner()))
			{
				if (ItemInfoWidgetInstanceA->Line1) ItemInfoWidgetInstanceA->Line1->SetText(FText::FromString(MeleeWeaponA->MeleeWeaponName));
				if (ItemInfoWidgetInstanceA->Line2) ItemInfoWidgetInstanceA->Line2->SetText(FText::FromString(MeleeWeaponA->MeleeWeaponHistory));
				if (ItemInfoWidgetInstanceA->Line3) ItemInfoWidgetInstanceA->Line3->SetText(FText::FromString(MeleeWeaponA->MeleeWeaponResistances));
				if (ItemInfoWidgetInstanceA->Line4) ItemInfoWidgetInstanceA->Line4->SetText(FText::FromString(MeleeWeaponA->MeleeWeaponWeaknesses));
				if (ItemInfoWidgetInstanceA->Line5) ItemInfoWidgetInstanceA->Line5->SetText(FText::FromString(MeleeWeaponA->MeleeWeaponDamage));
			}
			else if (ARangedWeapon* RangedWeaponA = Cast<ARangedWeapon>(ItemInfoWidgetComponentA->GetOwner()))
			{
				if (ItemInfoWidgetInstanceA->Line1) ItemInfoWidgetInstanceA->Line1->SetText(FText::FromString(RangedWeaponA->RangedWeaponName));
				if (ItemInfoWidgetInstanceA->Line2) ItemInfoWidgetInstanceA->Line2->SetText(FText::FromString(RangedWeaponA->RangedWeaponDescription));
				if (ItemInfoWidgetInstanceA->Line3) ItemInfoWidgetInstanceA->Line3->SetText(FText::FromString(RangedWeaponA->RangedWeaponType));
				if (ItemInfoWidgetInstanceA->Line4) ItemInfoWidgetInstanceA->Line4->SetText(FText::FromString(RangedWeaponA->RangedWeaponRarity));
				if (ItemInfoWidgetInstanceA->Line5) ItemInfoWidgetInstanceA->Line5->SetText(FText::FromString(RangedWeaponA->RangedWeaponDamage));
			}
		}
	}
}

void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ShowWidgets(true) called!"));
	
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingWeapon(this);
	}
	bShouldFloatSpin = false;
	AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
	bShouldHover = false;

	if (!OtherActor || OtherActor == this) return;

	// Optional: Cast to your player character class for strict type check
	auto* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player)
		ShowPickupAndInfoWidgets(true);

	// Fade in Info Widget
	if (ItemInfoWidgetComponentA)
	{
		UItemInfoWidgetBase* InfoInstanceA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (InfoInstanceA && InfoInstanceA->FadeInAnimation)
		{
			InfoInstanceA->PlayAnimation(InfoInstanceA->FadeInAnimation);
		}
	}
	
	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponentA)
	{
		UPickupGearWidget* PickupGearWidgetInstanceA = Cast<UPickupGearWidget>(PickupGearWidgetComponentA->GetUserWidgetObject());
		if (PickupGearWidgetInstanceA && PickupGearWidgetInstanceA->FadeInAnimation)
		{
			PickupGearWidgetInstanceA->PlayAnimation(PickupGearWidgetInstanceA->FadeInAnimation);
		}
	}
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingWeapon(nullptr);
	}

	if (!OtherActor || OtherActor == this) return;

	// Fade in Info Widget
	if (ItemInfoWidgetComponentA)
	{
		UItemInfoWidgetBase* InfoInstancedWidgetA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (InfoInstancedWidgetA && InfoInstancedWidgetA->FadeOutAnimation)
		{
			InfoInstancedWidgetA->PlayAnimation(InfoInstancedWidgetA->FadeOutAnimation);
		}
	}

	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponentA)
	{
		UPickupGearWidget* PickupGearInstancedWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponentA->GetUserWidgetObject());
		if (PickupGearInstancedWidgetA && PickupGearInstancedWidgetA->FadeOutAnimation)
		{
			PickupGearInstancedWidgetA->PlayAnimation(PickupGearInstancedWidgetA->FadeOutAnimation);
		}
	}	
	ShowPickupAndInfoWidgets(false);
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
	AreaSphere->DestroyComponent(); AreaSphere = nullptr;
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
	HoverDecal->SetVisibility(false);
	HoverLight->SetVisibility(false);

	// Safely destroy existing widgets
	if (PickupGearWidgetComponentA) { PickupGearWidgetComponentA->DestroyComponent(); PickupGearWidgetComponentA = nullptr; }
	if (ItemInfoWidgetComponentA) { ItemInfoWidgetComponentA->DestroyComponent(); ItemInfoWidgetComponentA = nullptr; }
}

void AWeaponBase::OnEquippedTwoHanded()
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
	if (ItemInfoWidgetComponentA) { ItemInfoWidgetComponentA->DestroyComponent(); ItemInfoWidgetComponentA = nullptr; }
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
		PickupGearWidgetComponentA->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PickupGearWidgetComponentA->SetWidgetClass(UPickupGearWidget::StaticClass());
		PickupGearWidgetComponentA->InitWidget();
		PickupGearWidgetComponentA->SetVisibility(true);
	}
	if (!ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
		ItemInfoWidgetComponentA->RegisterComponent();
		ItemInfoWidgetComponentA->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ItemInfoWidgetComponentA->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
		ItemInfoWidgetComponentA->InitWidget();
		ItemInfoWidgetComponentA->SetVisibility(true);
		ItemInfoWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
		ItemInfoWidgetComponentA->SetDrawSize(FVector2D(300.f, 200.f));
		ItemInfoWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
		ItemInfoWidgetComponentA->SetPivot(FVector2D(0.5f, 0.5f));
		ItemInfoWidgetComponentA->SetDrawAtDesiredSize(true);
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
	if (ItemInfoWidgetComponentA) { ItemInfoWidgetComponentA->DestroyComponent(); ItemInfoWidgetComponentA = nullptr; }
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

