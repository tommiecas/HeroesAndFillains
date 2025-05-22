// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"
#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "HUD/AmmoIntelWidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupGearWidget.h"

AAmmoPickup::AAmmoPickup()
{
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
	ItemInfoWidgetComponentA->SetVisibility(false, true);
	

	ItemInfoWidgetComponentB = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetB"));
	ItemInfoWidgetComponentB->SetupAttachment(RootComponent);
	ItemInfoWidgetComponentB->SetWidgetSpace(EWidgetSpace::World);
	ItemInfoWidgetComponentB->SetDrawSize(FVector2D(300.f, 100.f));
	ItemInfoWidgetComponentB->SetVisibility(false, true);

	const bool bIsVisibleA = ItemInfoWidgetComponentA->IsVisible();
	const bool bIsVisibleB = ItemInfoWidgetComponentB->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s AmmoPickupCOnstructor"), bIsVisibleA ? TEXT("Visible") : TEXT("Hidden"));
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s AmmoPickupConstructor"), bIsVisibleB ? TEXT("Visible") : TEXT("Hidden"));
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

	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(true);
		const bool bIsVisible = ItemInfoWidgetComponentA->IsVisible();
		UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s AmmoPickupOverlap"), bIsVisible ? TEXT("Visible") : TEXT("Hidden"));		
		UUserWidget* RawWidget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowWeaponAmmoFadeInAnimation(); 
			}
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB->SetVisibility(true);
		const bool bIsVisible = ItemInfoWidgetComponentA->IsVisible();
		UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentB visibility: %s AmmoPickupOverlap"), bIsVisible ? TEXT("Visible") : TEXT("Hidden"));
		
		UUserWidget* RawWidget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowWeaponAmmoFadeInAnimation(); 
			}
		}
	}
	
	bShouldFloatSpin = false;
	AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
	bShouldPickupHover = false;

	ShowPickupAndInfoWidgets(true);
	const bool bIsVisible = ItemInfoWidgetComponentA->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s AmmoPickup Overlap"), bIsVisible ? TEXT("Visible") : TEXT("Hidden"));


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
	if (ItemInfoWidgetComponentA)
	{
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowWeaponAmmoFadeOutAnimation(); 
			}
		}
	}
	if (ItemInfoWidgetComponentB)
	{
		ItemInfoWidgetComponentB->SetVisibility(false, true);
		UUserWidget* RawWidget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (RawWidget)
		{
			if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(RawWidget))
			{
				InfoWidget->ShowWeaponAmmoFadeOutAnimation(); 
			}
		}
	}
	ShowPickupAndInfoWidgets(false);
	const bool bIsVisible = ItemInfoWidgetComponentA->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s ammopickup overlap"), bIsVisible ? TEXT("Visible") : TEXT("Hidden"));

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

	// Use AddUniqueDynamic to safely bind
	AreaSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAmmoPickup::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AAmmoPickup::OnSphereEndOverlap);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	if (ItemInfoWidgetComponentA && !ItemInfoWidgetComponentA->GetWidgetClass())
	{
		ItemInfoWidgetComponentA->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentA->InitWidget();
		ItemInfoWidgetComponentA->SetVisibility(false, true);
		UUserWidget* Widget = ItemInfoWidgetComponentA->GetUserWidgetObject();
		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(Widget))
		{
			InfoWidget->SetItemInformation(
				GetAmmoNameText(),
				GetAmmoWeaponText(),
				GetAmmoDeliverableText(),
				GetAmmoAmountText(),
				GetAmmoDamageText()
			);
		}
	}
	if (ItemInfoWidgetComponentB && !ItemInfoWidgetComponentB->GetWidgetClass())
	{
		ItemInfoWidgetComponentB->SetWidgetClass(ItemInfoWidgetClass);
		ItemInfoWidgetComponentB->InitWidget();
		ItemInfoWidgetComponentB->SetVisibility(false, true);
		UUserWidget* Widget = ItemInfoWidgetComponentB->GetUserWidgetObject();
		if (UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(Widget))
		{
			InfoWidget->SetItemInformation(
				GetAmmoNameText(),
				GetAmmoWeaponText(),
				GetAmmoDeliverableText(),
				GetAmmoAmountText(),
				GetAmmoDamageText()	
			);
		}
	}
	
	if (PickupGearWidgetComponentA && !PickupGearWidgetComponentA->GetWidgetClass())
	{
		PickupGearWidgetComponentA->SetWidgetClass(PickupGearWidgetClass);
		PickupGearWidgetComponentA->InitWidget();
		PickupGearWidgetComponentA->SetVisibility(false, true);

		if (UUserWidget* RawWidget = PickupGearWidgetComponentA->GetUserWidgetObject())
		{
			if (UPickupGearWidget* PickupGearWidget = Cast<UPickupGearWidget>(RawWidget))
			{
				PickupGearWidget->OwningActor = this; // Or OwningAmmoPickup if you made it type-specific
			}
		}
	}
	if (PickupGearWidgetComponentB && !PickupGearWidgetComponentB->GetWidgetClass())
	{
		PickupGearWidgetComponentB->SetWidgetClass(PickupGearWidgetClass);
		PickupGearWidgetComponentB->InitWidget();
		PickupGearWidgetComponentB->SetVisibility(false, true);

		if (UUserWidget* RawWidget = PickupGearWidgetComponentB->GetUserWidgetObject())
		{
			if (UPickupGearWidget* PickupGearWidget = Cast<UPickupGearWidget>(RawWidget))
			{
				PickupGearWidget->OwningActor = this; // Or OwningAmmoPickup if you made it type-specific
			}
		}
	}
	ShowPickupAndInfoWidgets(false);
	const bool bIsVisible = ItemInfoWidgetComponentA->IsVisible();
	UE_LOG(LogTemp, Warning, TEXT("ItemInfoWidgetComponentA visibility: %s ammopickup beginplay"), bIsVisible ? TEXT("Visible") : TEXT("Hidden"));

}
