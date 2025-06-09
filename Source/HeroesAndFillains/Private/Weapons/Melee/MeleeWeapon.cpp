// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/WeaponBase.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponTypes.h"
#include "Weapons/Melee/ChaosSword.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMeleeWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
}

void AMeleeWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
}

void AMeleeWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	if (!InParent) return;
    
	// Make sure we're attaching to the skeletal mesh
	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(InParent))
	{
		if (!SkeletalMesh->DoesSocketExist(InSocketName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist on parent mesh"), *InSocketName.ToString());
			return;
		}
        
		// Set scale before attachment for ChaosSword
		if (Cast<AChaosSword>(this) && WeaponMesh)
		{
			WeaponMesh->SetRelativeScale3D(FVector(0.01f));
			UE_LOG(LogTemp, Warning, TEXT("ChaosSword Equip - Setting scale to 0.01"));
		}
        
		FAttachmentTransformRules TransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,  // Changed to SnapToTarget for scale
			true);
            
		WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
        
		// Ensure scale after attachment for ChaosSword
		if (Cast<AChaosSword>(this) && WeaponMesh)
		{
			WeaponMesh->SetRelativeScale3D(FVector(0.01f));
			UE_LOG(LogTemp, Warning, TEXT("ChaosSword Equip - After attachment scale: %s"), 
				*WeaponMesh->GetRelativeScale3D().ToString());
		}
	}
}

void AMeleeWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	Super::AttachMeshToSocket(InParent, InSocketName);
	
	if (!WeaponMesh || !InParent)
	{
		return;
	}

	// Store desired scale
	const FVector DesiredScale = Cast<AChaosSword>(this) ? FVector(0.01f) : WeaponMesh->GetRelativeScale3D();

	// Set world scale before attachment
	WeaponMesh->SetWorldScale3D(DesiredScale);

	FAttachmentTransformRules TransformRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::SnapToTarget,  // Scale - Changed from KeepWorld to SnapToTarget
		true
	);

	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
    
	// Force the scale after attachment
	WeaponMesh->SetRelativeScale3D(DesiredScale);  // Changed from SetWorldScale3D to SetRelativeScale3D
}




void AMeleeWeapon::OnEquippedOneHanded()
{
	Super::OnEquippedOneHanded();
}

void AMeleeWeapon::OnEquippedTwoHanded()
{
	Super::OnEquippedTwoHanded();

	if (WeaponMesh)
	{
		// Create and set up a socket for the left hand if it doesn't exist
		if (!WeaponMesh->DoesSocketExist(FName("LeftHandSocket")))
		{
			UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket does not exist on weapon mesh"));
			return;
		}

		// Get the socket transform
		FTransform LeftHandSocketTransform = WeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
        
		// Log the socket location for debugging
		UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket transform - Location: %s, Rotation: %s"), 
			*LeftHandSocketTransform.GetLocation().ToString(),
			*LeftHandSocketTransform.GetRotation().Rotator().ToString());
	}

}


void AMeleeWeapon::OnDropped()
{
	Super::OnDropped();
	
}

void AMeleeWeapon::OnEquippedSecondary()
{
	Super::OnEquippedSecondary();
}
	
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
}

void AMeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMeleeWeapon::SetEquippedMeleeWeaponState()
{
	if (MeleeType == EMeleeType::EMT_RubySword || MeleeType == EMeleeType::EMT_SapphireSword)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
	if (MeleeType == EMeleeType::EMT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
}

void AMeleeWeapon::SetMeleeWeaponInformationText(UWidgetComponent* MeleeWidgetComponent, AMeleeWeapon* MeleeWeapon)
{
		UItemInfoWidgetBase* MeleeWidget = Cast<UItemInfoWidgetBase>(MeleeWidgetComponent->GetUserWidgetObject());
		if (MeleeWeapon && MeleeWidget)
		{
			if (MeleeWidget->Line1) MeleeWidget->Line1->SetText(FText::FromString(MeleeWeapon->MeleeWeaponName));
			if (MeleeWidget->Line2) MeleeWidget->Line2->SetText(FText::FromString(MeleeWeapon->MeleeWeaponHistory));
			if (MeleeWidget->Line3) MeleeWidget->Line3->SetText(FText::FromString(MeleeWeapon->MeleeWeaponResistances));
			if (MeleeWidget->Line4) MeleeWidget->Line4->SetText(FText::FromString(MeleeWeapon->MeleeWeaponWeaknesses));
			if (MeleeWidget->Line5) MeleeWidget->Line5->SetText(FText::FromString(MeleeWeapon->MeleeWeaponDamage));
	}
}