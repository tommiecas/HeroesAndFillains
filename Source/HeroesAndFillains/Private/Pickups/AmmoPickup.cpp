// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"
#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/AmmoPickupIntelWidget.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupGearWidget.h"

AAmmoPickup::AAmmoPickup()
{
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
	
	PickupGearWidgetComponentB = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponentB"));
	PickupGearWidgetComponentB->SetupAttachment(Root);
	PickupGearWidgetComponentB->SetVisibility(false);
	PickupGearWidgetComponentB->SetWidgetSpace(EWidgetSpace::World);
	PickupGearWidgetComponentB->SetDrawSize(FVector2D(300.f, 50.f));
	PickupGearWidgetComponentB->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupGearWidgetComponentB->SetPivot(FVector2D(0.5f, 0.5f));
	PickupGearWidgetComponentB->SetDrawAtDesiredSize(true);

	
	ItemInfoWidgetComponentA = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponentA"));
	ItemInfoWidgetComponentA->SetupAttachment(Root);
	ItemInfoWidgetComponentA->SetVisibility(false);
	ItemInfoWidgetComponentA->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentA->SetDrawSize(FVector2D(300.f, 200.f));
	ItemInfoWidgetComponentA->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
	ItemInfoWidgetComponentA->SetPivot(FVector2D(0.5f, 0.5f));
	ItemInfoWidgetComponentA->SetDrawAtDesiredSize(true);
	
	ItemInfoWidgetComponentB = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponentB"));
	ItemInfoWidgetComponentB->SetupAttachment(Root);
	ItemInfoWidgetComponentB->SetVisibility(false);
	ItemInfoWidgetComponentB->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentB->SetDrawSize(FVector2D(300.f, 200.f));
	ItemInfoWidgetComponentB->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
	ItemInfoWidgetComponentB->SetPivot(FVector2D(0.5f, 0.5f));
	ItemInfoWidgetComponentB->SetDrawAtDesiredSize(true);
	
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

void AAmmoPickup::ShowPickupAndInfoWidgets(bool bShow)
{
	if (PickupGearWidgetComponentA) PickupGearWidgetComponentA->SetVisibility(bShow);
	if (PickupGearWidgetComponentB) PickupGearWidgetComponentB->SetVisibility(bShow);
	if (ItemInfoWidgetComponentA) ItemInfoWidgetComponentA->SetVisibility(bShow);
	if (ItemInfoWidgetComponentB) ItemInfoWidgetComponentB->SetVisibility(bShow);
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	bShouldFloatSpin = false;
	AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
	bShouldPickupHover = false;

	if (!OtherActor || OtherActor == this) return;

	// Optional: Cast to your player character class for strict type check
	auto* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player)
		ShowPickupAndInfoWidgets(true);

	UE_LOG(LogTemp, Warning, TEXT("Entered proximity of weapon: %s"), *GetName());

	// Fade in Info Widget
	if (ItemInfoWidgetComponentA)
	{
		UItemInfoWidgetBase* InfoInstanceA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (InfoInstanceA && InfoInstanceA->FadeInAnimation)
		{
			InfoInstanceA->PlayAnimation(InfoInstanceA->FadeInAnimation);
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		UItemInfoWidgetBase* InfoInstanceB = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentB->GetUserWidgetObject());
		if (InfoInstanceB && InfoInstanceB->FadeInAnimation)
		{
			InfoInstanceB->PlayAnimation(InfoInstanceB->FadeInAnimation);
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
	if (PickupGearWidgetComponentB)
	{
		UPickupGearWidget* PickupGearWidgetInstanceB = Cast<UPickupGearWidget>(PickupGearWidgetComponentB->GetUserWidgetObject());
		if (PickupGearWidgetInstanceB && PickupGearWidgetInstanceB->FadeInAnimation)
		{
			PickupGearWidgetInstanceB->PlayAnimation(PickupGearWidgetInstanceB->FadeInAnimation);
		}
	}

	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		UCombatComponent* Combat = FillainCharacter->GetCombatComponent();
		if (Combat)
		{
			Combat->PickupAmmo(RangedType, AmmoAmount);
		}
	}
	Destroy();
}

void AAmmoPickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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
	if (ItemInfoWidgetComponentB)
	{
		UItemInfoWidgetBase* InfoInstancedWidgetB = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentB->GetUserWidgetObject());
		if (InfoInstancedWidgetB && InfoInstancedWidgetB->FadeOutAnimation)
		{
			InfoInstancedWidgetB->PlayAnimation(InfoInstancedWidgetB->FadeOutAnimation);
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
	// Fade in Equip Widget (if you have a subclass for it)
	if (PickupGearWidgetComponentB)
	{
		UPickupGearWidget* PickupGearInstancedWidgetB = Cast<UPickupGearWidget>(PickupGearWidgetComponentB->GetUserWidgetObject());
		if (PickupGearInstancedWidgetB && PickupGearInstancedWidgetB->FadeOutAnimation)
		{
			PickupGearInstancedWidgetB->PlayAnimation(PickupGearInstancedWidgetB->FadeOutAnimation);
		}
	}
	ShowPickupAndInfoWidgets(false);

	UE_LOG(LogTemp, Warning, TEXT("Exited proximity of weapon: %s"), *GetName());
}

void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AAmmoPickup::TransformedSin() const
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AAmmoPickup::TransformedCos() const
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
	// Use AddUniqueDynamic to safely bind
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickup::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AAmmoPickup::OnSphereEndOverlap);
	
	ShowPickupAndInfoWidgets(false);

	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetInstanceA = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentA->GetUserWidgetObject());
		if (ItemInfoWidgetInstanceA)
		{
			ItemInfoWidgetInstanceA->SetRangedInfo(Line1, Line2, Line3, Line4, Line5);
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetInstanceB = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponentB->GetUserWidgetObject());
		if (ItemInfoWidgetInstanceB)
		{
			ItemInfoWidgetInstanceB->SetRangedInfo(Line1, Line2, Line3, Line4, Line5);
		}
	}
}
