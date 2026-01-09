// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/MajixWeapon.h"

#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Enemies/EnemyBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HAFComponents/CombatComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Majix/HAFMajixProjectile.h"


AMajixWeapon::AMajixWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	NewSphere = CreateDefaultSubobject<USphereComponent>(TEXT("NewSphere"));
	SetRootComponent(NewSphere);
	NewSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NewSphere->SetCollisionObjectType(ECC_Projectile);
	NewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	NewSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	NewSphere->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	NewSphere->IgnoreActorWhenMoving(this, true);
	
	InitializeMajixWeaponMeshIfNotProjectile();
}

AMajixWeapon* AMajixWeapon::InitializeMajixWeaponMeshIfNotProjectile()
{
	// If this actor is NOT a projectile
	if (!IsA(AHAFMajixProjectile::StaticClass()))
	{
		WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MajixWeaponMesh"));
		SetRootComponent(WeaponMesh);
		EFightingStyle FightingStyle = EFightingStyle::EFS_Majix;
		if (FightingStyle == EFightingStyle::EFS_Majix)
		{
			return this; 
		}
	}
	else if (IsA(AHAFMajixProjectile::StaticClass()))
	{
		EFightingStyle FightingStyle = EFightingStyle::EFS_Majix;
		if (FightingStyle == EFightingStyle::EFS_Majix)
		{
			return this; 
		}
	}
	return nullptr;
}


void AMajixWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMajixWeapon::BeginPlay()
{
	Super::BeginPlay();

	/* Validate WeaponBox
	if (!WeaponBox)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ WeaponBox is null in %s"), *GetName());
		return;
	}*/

	// Initial Debug Color
	// FColor DebugColor = FColor::Red;

	// Check owner and tags
	// AActor* WeaponOwner = GetOwner();
	// if (IsValid(WeaponOwner))
	// {
	// 	if (WeaponOwner->Tags.Contains("Enemy"))
	// 	{
			/* DebugColor = FColor::Blue;
			UE_LOG(LogTemp, Warning, TEXT("✅ WeaponOwner '%s' has 'Enemy' tag"), *WeaponOwner->GetName());
		}
		else if (WeaponOwner->Tags.Contains("Player"))
		{
			DebugColor = FColor::Green;
			UE_LOG(LogTemp, Warning, TEXT("✅ WeaponOwner '%s' has 'Player' tag"), *WeaponOwner->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ WeaponOwner '%s' has no recognized tag"), *WeaponOwner->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Weapon has no valid owner"));
	}*/

			// Collision Setup
			
			// Log Collision Info
			// UE_LOG(LogTemp, Warning, TEXT("📦 WeaponBox Setup"));
			// UE_LOG(LogTemp, Warning, TEXT(" - ObjectType: %d"), WeaponBox->GetCollisionObjectType());
			// UE_LOG(LogTemp, Warning, TEXT(" - Response to ECC_PlayerCharacter: %d"), WeaponBox->GetCollisionResponseToChannel(ECC_PlayerCharacter));
			// UE_LOG(LogTemp, Warning, TEXT(" - Collision Profile: %s"), *WeaponBox->GetCollisionProfileName().ToString());

			// Attach Box to Mesh
			
			
			// UE_LOG(LogTemp, Warning, TEXT("🔗 Overlap delegate bound: %d"), WeaponBox->OnComponentBeginOverlap.IsBound());

			/* Debug Box (visual)
			DrawDebugBox(
				GetWorld(),
				WeaponBox->GetComponentLocation(),
				WeaponBox->GetScaledBoxExtent(),
				WeaponBox->GetComponentQuat(),
				DebugColor,
				false,
				5.0f,
				0,
				2.0f 
			); */


	
			/* Attachment verification
			if (WeaponBox->GetAttachParent() != WeaponMesh)
			{
				UE_LOG(LogTemp, Error, TEXT("❌ WeaponBox is not attached to WeaponMesh!"));
			}*/
}

void AMajixWeapon::Equip(USkeletalMeshComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	if (!InParent) return;
    
	// Make sure we're attaching to the skeletal mesh
	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(InParent))
	{
		if (!SkeletalMesh->DoesSocketExist(InSocketName)) return;
		if (NewOwner->ActorHasTag(FName("Fillain")))
		{
			const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("MajixSocket"));
			if (HandSocket)
			{
				if (AFillainCharacter* AFC = Cast<AFillainCharacter>(NewOwner))
				{
					HandSocket->AttachActor(AFC->GetEquippedMajixWeapon(), Character->GetMesh());	
				}
			}
		}
		else if (NewOwner->ActorHasTag(FName("Enemy")))
		{
			AEnemyBase* BaseEnemy = Cast<AEnemyBase>(NewOwner);
			const USkeletalMeshSocket* HandSocket = BaseEnemy->GetMesh()->GetSocketByName(FName("MajixSocket"));
			if (HandSocket)
			{
				HandSocket->AttachActor(BaseEnemy->EquippedEnemyMajixWeapon, BaseEnemy->GetMesh());
			}
		}
	}
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
	ItemState = EItemState::EIS_Equipped;
	bIsEquipped = true;
	SetOwner(NewOwner);
	ShowPickupAndInfoWidgets(false);
	SetInstigator(NewInstigator);
	if (NewOwner->ActorHasTag(FName("Fillain")) && IsValid(NewOwner))
	{
		if (AFillainCharacter* FillCharacter = Cast<AFillainCharacter>(NewOwner))
		{
			AttachMeshToSocket(InParent, InSocketName);
			PlayEquipSound();
			SetHandsNeeded(this);
			FillCharacter->GetEquippedMajixWeapon()->SetEquippedWeaponState();
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
	}
	else if (NewOwner->ActorHasTag(FName("Enemy")) && IsValid(NewOwner))
	{
		if (AEnemyBase* BE = Cast<AEnemyBase>(NewOwner))
		{
			AttachMeshToSocket(InParent, InSocketName);
			SetHandsNeeded(this);
			BE->GetEquippedMajixWeapon()->SetEquippedWeaponState();
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
		}
	}
}

void AMajixWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
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

void AMajixWeapon::SetEquippedWeaponState()
{
	if (MaJixType == EMaJixType::EJT_FerattaStaff || MaJixType == EMaJixType::EJT_ShamanStaff || MaJixType == EMaJixType::EJT_FireBolt || MaJixType == EMaJixType::EJT_MajixProjectile)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
	}
	else MaJixType = EMaJixType::EJT_None;
}

void AMajixWeapon::OnEquippedOneHanded()
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
			NewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnableCustomDepth(false);
	
			if (HoveringDecal) HoveringDecal->SetVisibility(false);
			if (HoveringLight) HoveringLight->SetVisibility(false);
		},
		0.1f,
		false
	);
	// Safely destroy existing widgets
	if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
	if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
}

void AMajixWeapon::OnEquippedTwoHanded()
{
	if (WeaponMesh)
	{
		// Create and set up a socket for the left hand if it doesn't exist
		if (!WeaponMesh->DoesSocketExist(FName("LeftHandSocket")))
		{
			// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket does not exist on weapon mesh"));
			return;
		}

		// Get the socket transform
		FTransform LeftHandSocketTransform = WeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
        
		// Log the socket location for debugging
		// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket transform - Location: %s, Rotation: %s"), 
		//			*LeftHandSocketTransform.GetLocation().ToString(),
		//			*LeftHandSocketTransform.GetRotation().Rotator().ToString());;
		
		NewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
				NewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				EnableCustomDepth(false);
	
				if (HoveringDecal) HoveringDecal->SetVisibility(false);
				if (HoveringLight) HoveringLight->SetVisibility(false);
			},
			0.1f,
			false
		);
		// Safely destroy existing widgets
		if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
		if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
	}
}




