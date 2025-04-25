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

AWeaponFinal::AWeaponFinal()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh")); 
	SetRootComponent(WeaponMesh);


	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickupWidgetA = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetA"));
	PickupWidgetA->SetupAttachment(RootComponent);

	PickupWidgetB = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetB"));
	PickupWidgetB->SetupAttachment(RootComponent);

	NameWidget1 = CreateDefaultSubobject<UPickupWidgetComponent>(TEXT("NameWidget1"));
	NameWidget1->SetupAttachment(RootComponent);

	NameWidget2 = CreateDefaultSubobject<UPickupWidgetComponent>(TEXT("NameWidget2"));
	NameWidget2->SetupAttachment(RootComponent);
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
	if (PickupWidgetA)
	{
		PickupWidgetA->SetVisibility(false);
	}
	if (PickupWidgetB)
	{
		PickupWidgetB->SetVisibility(false);
	}
	if (NameWidget1)
	{
		NameWidget1->SetVisibility(false);
	}
	if (NameWidget2)
	{
		NameWidget2->SetVisibility(false);
	}
}

void AWeaponFinal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}
}

void AWeaponFinal::OnSphereEndOverlap(UPrimitiveComponent* OverlappingCOmponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(OtherActor);
	if (FillainCharacter)
	{
		FillainCharacter->SetOverlappingWeaponFinal(nullptr);
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
	WeaponFinalState = State;
	switch (WeaponFinalState)
	{
	case EWeaponFinalState::EWFS_Equipped:
		ShowPickupAndNameWidgets(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponFinalState::EWFS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}
	
void AWeaponFinal::OnRep_WeaponFinalState()
{
	switch (WeaponFinalState)
	{
	case EWeaponFinalState::EWFS_Equipped:
		ShowPickupAndNameWidgets(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponFinalState::EWFS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeaponFinal::ShowPickupAndNameWidgets(bool bShowPickupAndNameWidgets)
{
	if (PickupWidgetA)
	{
		PickupWidgetA->SetVisibility(bShowPickupAndNameWidgets);
	}
	if (PickupWidgetB)
	{
		PickupWidgetB->SetVisibility(bShowPickupAndNameWidgets);
	}
	if (NameWidget1)
	{
		NameWidget1->SetVisibility(bShowPickupAndNameWidgets);
	}
	if (NameWidget2)
	{
		NameWidget2->SetVisibility(bShowPickupAndNameWidgets);
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

FString AWeaponFinal::GetWeaponFinalTypeDisplayed(EWeaponFinalType TypeOfWeaponFinal)
{
	const UEnum* EnumPtr = StaticEnum<EWeaponFinalType>();
	if (!EnumPtr) return FString("Invalid");

	if (TypeOfWeaponFinal == EWeaponFinalType::EWFT_None || TypeOfWeaponFinal == EWeaponFinalType::EWFT_MAX) return FString("None");
	
	FString DisplayName = EnumPtr->GetDisplayNameTextByValue((int64)TypeOfWeaponFinal).ToString();
	DisplayWeaponFinalName(DisplayName);
	return DisplayName;
}

void AWeaponFinal::DisplayWeaponFinalName(FString Name)
{
	switch (WeaponFinalType)
	{//FText::FromString(TEXT("Assault Rifle")).ToString()
	case EWeaponFinalType::EWFT_AssaultRifle:
		NameWidget1->SetWeaponNameText(Name, this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(Name, this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_RocketLauncher:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Rocket Launcher")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Rocket Launcher")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_Pistol:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Pistol")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Pistol")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_SubmachineGun:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Sub-Machine Gun")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Sub-Machine Gun")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_Shotgun:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Shotgun")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Shotgun")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_SniperRifle:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Sniper Rifle")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Sniper Rifle")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_GrenadeLauncher:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Grenade Launcher")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Grenade Launcher")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	case EWeaponFinalType::EWFT_Sword:
		NameWidget1->SetWeaponNameText(FText::FromString(TEXT("Sword")).ToString(), this);
		NameWidget1->ShowWeaponFinalName(this);
		NameWidget2->SetWeaponNameText(FText::FromString(TEXT("Sword")).ToString(), this);
		NameWidget2->ShowWeaponFinalName(this);
		break;
	default:
		break;
	}
}