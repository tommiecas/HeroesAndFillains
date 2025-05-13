// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/WeaponFinal.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "HUD/PickupWidget.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponsFinal/CasingFinal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "WeaponsFinal/WeaponsFinalTypes.h"
#include "HUD/PickupWidgetComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/PointLightComponent.h"
#include "Components/DecalComponent.h"
#include "HUD/WeaponInfoWidget.h"

AWeaponFinal::AWeaponFinal()
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

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	
	PickupWidgetA = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetA"));
	PickupWidgetA->SetupAttachment(RootComponent);

	PickupWidgetB = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetB"));
	PickupWidgetB->SetupAttachment(RootComponent);

	WeaponInfoWidget1 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponInfoWidget1"));
	WeaponInfoWidget1->SetupAttachment(RootComponent);
	WeaponInfoWidget1->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	WeaponInfoWidget1->SetDrawSize(FVector2D(300.f, 100.f));
	WeaponInfoWidget1->SetWidgetClass(UWeaponInfoWidget::StaticClass()); // Set the widget class to your custom widget class

	WeaponInfoWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponInfoWidget2"));
	WeaponInfoWidget2->SetupAttachment(RootComponent);
	WeaponInfoWidget2->SetWidgetSpace(EWidgetSpace::Screen);       // Widget rendered in screen space (or use World if 3D)
	WeaponInfoWidget2->SetDrawSize(FVector2D(300.f, 100.f));
	WeaponInfoWidget2->SetWidgetClass(UWeaponInfoWidget::StaticClass()); // Set the widget class to your custom widget classWidget2 = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget2"));

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



void AWeaponFinal::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

FVector AWeaponFinal::TraceEndWithScatter(const FVector& HitTarget)
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

void AWeaponFinal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponFinal::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponFinal::OnSphereEndOverlap);
	}

	
	if (WeaponInfoWidget1)
	{
		UUserWidget* UserWidget = WeaponInfoWidget1->GetUserWidgetObject();
		UWeaponInfoWidget* WeaponInfoWidget = Cast<UWeaponInfoWidget>(UserWidget);
		if (WeaponInfoWidget)
		{
			FText TypeText = GetWeaponTypeText();
			// Pass the values to the widget
			WeaponInfoWidget->SetWeaponInfo(WeaponName, WeaponDescription, TypeText, WeaponRarity, WeaponDamage);
		}
		WeaponInfoWidget1->SetVisibility(false);
	}
	if (WeaponInfoWidget2)
	{
		UUserWidget* UserWidget = WeaponInfoWidget2->GetUserWidgetObject();
		UWeaponInfoWidget* WeaponInfoWidget = Cast<UWeaponInfoWidget>(UserWidget);
		if (WeaponInfoWidget)
		{
			FText TypeText = GetWeaponTypeText();
			// Pass the values to the widget
			WeaponInfoWidget->SetWeaponInfo(WeaponName, WeaponDescription, TypeText, WeaponRarity, WeaponDamage);
		}
		WeaponInfoWidget2->SetVisibility(false);
	}
	if (PickupWidgetA) PickupWidgetA->SetVisibility(false);
	if (PickupWidgetB) PickupWidgetB->SetVisibility(false);
}

void AWeaponFinal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bShouldHover)
	{
		float RunningTime = GetGameTimeSinceCreation(); // How long this actor has existed
		float DeltaHeight = FMath::Sin(RunningTime * HoverSpeed) * HoverAmplitude * DeltaTime;

		FVector NewLocation = GetActorLocation();
		NewLocation.Z += DeltaHeight;
		SetActorLocation(NewLocation);
	}

	if (bShouldFloatSpin) // You can make a bool for it
	{
		AddActorLocalRotation(FRotator(0.f, 30.f * DeltaTime, 0.f)); // 30 degrees per second
	}
}

void AWeaponFinal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponFinal, WeaponFinalState);
	DOREPLIFETIME(AWeaponFinal, Ammo);
}

void AWeaponFinal::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		FillainCharacter->SetOverlappingWeaponFinal(this);
		bShouldFloatSpin = false;
		AddActorLocalRotation(FRotator(0.f, 0.f, 0.f)); 
		bShouldHover = false;
		if (WeaponInfoWidget1)
		{
			WeaponInfoWidget1->SetVisibility(true);
		}
		if (WeaponInfoWidget2)
		{
			WeaponInfoWidget2->SetVisibility(true);
		}
		if (PickupWidgetA)
		{
			PickupWidgetA->SetVisibility(true);
		}
		if (PickupWidgetB)
		{
			PickupWidgetB->SetVisibility(true);
		}
	}

}

void AWeaponFinal::OnSphereEndOverlap(UPrimitiveComponent* OverlappingCOmponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		FillainCharacter->SetOverlappingWeaponFinal(nullptr);
	}
	if (WeaponInfoWidget1)
	{
		WeaponInfoWidget1->SetVisibility(false);
	}
	if (WeaponInfoWidget2)
	{
		WeaponInfoWidget1->SetVisibility(false);
	}
	if (PickupWidgetA)
	{
		PickupWidgetA->SetVisibility(false);
	}
	if (PickupWidgetB)
	{
		PickupWidgetB->SetVisibility(false);
	}
}

void AWeaponFinal::SetHUDAmmo()
{
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;


	if (FillainOwnerCharacter)
	{
		FillainOwnerController = FillainOwnerController == nullptr ? Cast<AFillainPlayerController>(FillainOwnerCharacter->Controller) : FillainOwnerController;


		if (FillainOwnerController)
		{
			FillainOwnerController->SetHUDWeaponFinalAmmo(Ammo);
		}
	}
}

void AWeaponFinal::SpendRoundOfAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();


}

void AWeaponFinal::OnRep_Ammo()
{
	FillainOwnerCharacter = FillainOwnerCharacter == nullptr ? Cast<AFillainCharacter>(GetOwner()) : FillainOwnerCharacter;
	SetHUDAmmo();
}

void AWeaponFinal::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		FillainOwnerCharacter = nullptr;
		FillainOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void AWeaponFinal::SetWeaponFinalState(EWeaponFinalState State)
{
}	
	
FText AWeaponFinal::GetWeaponTypeText() const
{
	// Convert the EWeaponFinalType enum value to a localized FText using the display name
	const UEnum* EnumPtr = StaticEnum<EWeaponFinalType>();
	if (EnumPtr) 
	{
		// Get display name from enum (uses UMETA(DisplayName) if provided)
		return EnumPtr->GetDisplayNameTextByValue((int64)WeaponFinalType);
	}
	// Fallback text if enum is invalid
	return FText::FromString("Unknown");;
}

void AWeaponFinal::OnRep_WeaponFinalState()
{
	switch (WeaponFinalState)
	{
	case EWeaponFinalState::EWFS_Equipped:
		bShouldHover = false;
		bShouldFloatSpin = false;
		ShowPickupAndWeaponInfoWidgets(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponFinalType == EWeaponFinalType::EWFT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		EnableCustomDepth(false); // Enable custom depth rendering for the mesh
		HoverDecal->SetVisibility(false);
		HoverLight->SetVisibility(false);
		break;

	case EWeaponFinalState::EWFS_Dropped:
		bShouldHover = true;
		bShouldFloatSpin = true;
		ShowPickupAndWeaponInfoWidgets(true);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
		WeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
		EnableCustomDepth(true); // Enable custom depth rendering for the mesh
		HoverDecal->SetVisibility(true);
		HoverLight->SetVisibility(true);
		break;
	}
}

void AWeaponFinal::ShowPickupAndWeaponInfoWidgets(bool bShowPickupAndWeaponInfoWidgets)
{
	if (PickupWidgetA)
	{
		PickupWidgetA->SetVisibility(bShowPickupAndWeaponInfoWidgets);
	}
	if (PickupWidgetB)
	{
		PickupWidgetB->SetVisibility(bShowPickupAndWeaponInfoWidgets);
	}
	if (WeaponInfoWidget1)
	{
		WeaponInfoWidget1->SetVisibility(bShowPickupAndWeaponInfoWidgets);
	}
	if (WeaponInfoWidget2)
	{
		WeaponInfoWidget2->SetVisibility(bShowPickupAndWeaponInfoWidgets);
	}
}

void AWeaponFinal::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingFinalClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasingFinal>(
					CasingFinalClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	SpendRoundOfAmmo();
}

void AWeaponFinal::WeaponFinalDropped()
{
	SetWeaponFinalState(EWeaponFinalState::EWFS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	FillainOwnerCharacter = nullptr;
	FillainOwnerController = nullptr;
}

void AWeaponFinal::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();

}

bool AWeaponFinal::IsWeaponFinalEmpty()
{
	return (Ammo <= 0);
}

bool AWeaponFinal::IsWeaponFinalFull()
{
	return Ammo == MagCapacity;
}

