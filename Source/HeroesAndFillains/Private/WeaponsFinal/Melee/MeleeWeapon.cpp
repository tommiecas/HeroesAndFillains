// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/Melee/MeleeWeapon.h"
#include "Components/SphereComponent.h"
#include "Weapons/WeaponTypes.h"
#include "Components/WidgetComponent.h"
#include "HUD/WeaponInfoWidget.h"
#include "HUD/PickupWidget.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>

#include "Characters/FillainCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "HUD/MeleeInfoWidget.h"
#include "Net/UnrealNetwork.h"
#include "WeaponsFinal/WeaponFinal.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	MeleeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeMesh"));
	MeleeMesh->SetupAttachment(RootComponent);
	MeleeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeleeMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	MeleeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	MeleeMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	EnableCustomDepth(true); // Enable custom depth rendering for the mesh

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);
	
	PickupWidgetA = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetA"));
	PickupWidgetA->SetupAttachment(RootComponent);

	PickupWidgetB = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetB"));
	PickupWidgetB->SetupAttachment(RootComponent);

	MeleeInfoWidget1 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponInfoWidget1"));
	MeleeInfoWidget1->SetupAttachment(RootComponent);
	MeleeInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	MeleeInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
	MeleeInfoWidget1->SetWidgetClass(UWeaponInfoWidget::StaticClass()); // Set the widget class to your custom widget class

	MeleeInfoWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponInfoWidget2"));
	MeleeInfoWidget2->SetupAttachment(RootComponent);
	MeleeInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	MeleeInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
	MeleeInfoWidget2->SetWidgetClass(UWeaponInfoWidget::StaticClass()); // Set the widget class to your custom widget classWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget2"));

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

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeWeapon::OnSphereEndOverlap);

	if (HasAuthority())
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnSphereOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeWeapon::OnSphereEndOverlap);
	}

	if (MeleeInfoWidget1)
	{
		UUserWidget* UserWidget = MeleeInfoWidget1->GetUserWidgetObject();
		UMeleeInfoWidget* MeleeWidget = Cast<UMeleeInfoWidget>(UserWidget);
		if (MeleeWidget)
		{
			FText NameText = GetMeleeWeaponNameText();
			// Pass the values to the widget
			MeleeWidget->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		}
		MeleeInfoWidget1->SetVisibility(false);
	}
	if (MeleeInfoWidget2)
	{
		UUserWidget* UserWidget = MeleeInfoWidget2->GetUserWidgetObject();
		if (UMeleeInfoWidget* MeleeWeaponInfoWidget = Cast<UMeleeInfoWidget>(UserWidget))
		{
			FText NameText = GetMeleeWeaponNameText();
			// Pass the values to the widget
			MeleeWeaponInfoWidget->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
		}
		MeleeInfoWidget2->SetVisibility(false);
	}
	if (PickupWidgetA) PickupWidgetA->SetVisibility(false);
	if (PickupWidgetB) PickupWidgetB->SetVisibility(false);
}

void AMeleeWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingMeleeWeapon(this);
	}
}

void AMeleeWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor))
	{
		FillainCharacter->SetOverlappingMeleeWeapon(nullptr);
	}
}

float AMeleeWeapon::TransformedSin() const
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AMeleeWeapon::TransformedCos() const
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

FText AMeleeWeapon::GetMeleeWeaponNameText() const
{
	// Convert the EWeaponFinalType enum value to a localized FText using the display name
	const UEnum* EnumPtr = StaticEnum<EMeleeType>();
	if (EnumPtr) 
	{
		// Get display name from enum (uses UMETA(DisplayName) if provided)
		return EnumPtr->GetDisplayNameTextByValue((int64)MeleeType);
	}
	// Fallback text if enum is invalid
	return FText::FromString("Unknown");;
}

void AMeleeWeapon::OnRep_MeleeState()
{
}

void AMeleeWeapon::SetMeleeState(EMeleeState State)
{
	switch (State)
	{
	case EMeleeState::EMS_Equipped:
	case EMeleeState::EMS_EquippedSecondary:
		bShouldHover = false;
		bShouldFloatSpin = false;
		ShowPickupAndMeleeInfoWidgets(false);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeleeMesh->SetSimulatePhysics(false);
		MeleeMesh->SetEnableGravity(false);
		MeleeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EnableCustomDepth(false);
		if (HoverDecal) HoverDecal->SetVisibility(false);
		if (HoverLight) HoverLight->SetVisibility(false);
        
		// Safely destroy existing widgets
		if (PickupWidgetA) { PickupWidgetA->DestroyComponent(); PickupWidgetA = nullptr; }
		if (PickupWidgetB) { PickupWidgetB->DestroyComponent(); PickupWidgetB = nullptr; }
		if (MeleeInfoWidget1) { MeleeInfoWidget1->DestroyComponent(); MeleeInfoWidget1 = nullptr; }
		if (MeleeInfoWidget2) { MeleeInfoWidget2->DestroyComponent(); MeleeInfoWidget2 = nullptr; }
		break;

	case EMeleeState::EMS_Dropped:
		bShouldHover = true;
		bShouldFloatSpin = true;
		MeleeMesh->SetSimulatePhysics(true);
		MeleeMesh->SetEnableGravity(true);
		MeleeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (HasAuthority())
		{
			Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		MeleeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		MeleeMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		MeleeMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
		MeleeMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		MeleeMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		if (HoverDecal) HoverDecal->SetVisibility(true);
		if (HoverLight) HoverLight->SetVisibility(true);

		// Create PipckupWidgetA
		if (!PickupWidgetA)
		{
			PickupWidgetA = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetA"));
			if (PickupWidgetA)
			{
				PickupWidgetA->RegisterComponent();
				PickupWidgetA->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}

		// Create PickupWidgetB
		if (!PickupWidgetB)
		{
			PickupWidgetB = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetB"));
			if (PickupWidgetB)
			{
				PickupWidgetB->RegisterComponent();
				PickupWidgetB->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}
		
		// Create MeleeInfoWidget1
        if (!MeleeInfoWidget1)
        {
            MeleeInfoWidget1 = NewObject<UWidgetComponent>(this, TEXT("MeleeInfoWidget1"));
            if (MeleeInfoWidget1)
            {
                MeleeInfoWidget1->RegisterComponent();
                MeleeInfoWidget1->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeleeInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);
                MeleeInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
                MeleeInfoWidget1->SetWidgetClass(UWeaponInfoWidget::StaticClass());
                
                // Initialize widget content
                if (UUserWidget* UserWidget = MeleeInfoWidget1->GetUserWidgetObject())
                {
                    if (UMeleeInfoWidget* MeleeWidget = Cast<UMeleeInfoWidget>(UserWidget))
                    {
                        MeleeWidget->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, 
                            MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
                    }
                }
            }
        }

        // Create MeleeInfoWidget2
        if (!MeleeInfoWidget2)
        {
            MeleeInfoWidget2 = NewObject<UWidgetComponent>(this, TEXT("MeleeInfoWidget2"));
            if (MeleeInfoWidget2)
            {
                MeleeInfoWidget2->RegisterComponent();
                MeleeInfoWidget2->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeleeInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);
                MeleeInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
                MeleeInfoWidget2->SetWidgetClass(UWeaponInfoWidget::StaticClass());
                
                // Initialize widget content
                if (UUserWidget* UserWidget = MeleeInfoWidget2->GetUserWidgetObject())
                {
                    if (UMeleeInfoWidget* MeleeWidget = Cast<UMeleeInfoWidget>(UserWidget))
                    {
                        MeleeWidget->SetMeleeWeaponInfo(MeleeWeaponName, MeleeWeaponHistory, 
                            MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
                    }
                }
            }
        }
        break;
    }

	if (FloatingWidgetComponent)
	{
		FloatingWidgetComponent->SetVisibility(false);
	}
}


void AMeleeWeapon::ShowPickupAndMeleeInfoWidgets(bool bShowPickupAndMeleeInfoWidgets)
{
	if (PickupWidgetA)
	{
		PickupWidgetA->SetVisibility(bShowPickupAndMeleeInfoWidgets);
	}
	if (PickupWidgetB)
	{
		PickupWidgetB->SetVisibility(bShowPickupAndMeleeInfoWidgets);
	}
	if (MeleeInfoWidget1)
	{
		MeleeInfoWidget1->SetVisibility(bShowPickupAndMeleeInfoWidgets);
	}
	if (MeleeInfoWidget2)
	{
		MeleeInfoWidget2->SetVisibility(bShowPickupAndMeleeInfoWidgets);
	}
}


void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
}

void AMeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMeleeWeapon, MeleeState);

}

void AMeleeWeapon::EnableCustomDepth(const bool bEnable) const
{
	if (MeleeMesh)
	{
		MeleeMesh->SetRenderCustomDepth(bEnable);
	}
}