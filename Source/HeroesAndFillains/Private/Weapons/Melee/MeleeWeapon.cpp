// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "NiagaraComponent.h"
#include "Characters/FillainCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/WeaponBase.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponTypes.h"
#include "Weapons/Melee/ChaosSword.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(Root);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TracePointHilt = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointHilt"));
	TracePointHilt->SetupAttachment(WeaponMesh);

	TracePointMid = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointMid"));
	TracePointMid->SetupAttachment(WeaponMesh);

	TracePointTip = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointTip"));
	TracePointTip->SetupAttachment(WeaponMesh);

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

void AMeleeWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	if (!InParent) return;
    
	// Make sure we're attaching to the skeletal mesh
	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(InParent))
	{
		if (!SkeletalMesh->DoesSocketExist(InSocketName))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist on parent mesh"), *InSocketName.ToString());
			return;
		}
        
		SetOwner(NewOwner);
        SetInstigator(NewInstigator);
		AttachMeshToSocket(InParent, InSocketName);
        ItemState = EItemState::EIS_Equipped;
		if (EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
		}
		if (AreaSphere)
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (EmbersEffect)
		{
			EmbersEffect->Deactivate();
		}
		AActor* OwnerCharacter = GetOwner(); // Typically set on equip

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this); // Ignore the weapon itself
		if (OwnerCharacter)
		{
			TraceParams.AddIgnoredActor(OwnerCharacter); // ✅ Ignore the wielder!
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

	

	FAttachmentTransformRules TransformRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::SnapToTarget,  // Scale - Changed from KeepWorld to SnapToTarget
		true
	);

	WeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
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
			// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket does not exist on weapon mesh"));
			return;
		}

		// Get the socket transform
		FTransform LeftHandSocketTransform = WeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
        
		// Log the socket location for debugging
		// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket transform - Location: %s, Rotation: %s"), 
//			*LeftHandSocketTransform.GetLocation().ToString(),
//			*LeftHandSocketTransform.GetRotation().Rotator().ToString());;
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

void AMeleeWeapon::BeginAttack()
{
	bIsTracing = true;

	LastTraceLocationTip  = TracePointTip->GetComponentLocation();
	LastTraceLocationMid  = TracePointMid->GetComponentLocation();
	LastTraceLocationHilt = TracePointHilt->GetComponentLocation();

	IgnoreActors.Empty();

	// UE_LOG(LogTemp, Warning, TEXT("🗡 Melee Trace Started"));
}

void AMeleeWeapon::TraceBetweenPoints(FVector& LastLocation, USceneComponent* TracePoint)
{
	FVector CurrentLocation = TracePoint->GetComponentLocation();
	FHitResult Hit;

	TArray<AActor*> ActorsToIgnore = { this, GetOwner() };

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		LastLocation,
		CurrentLocation,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true
	);
	AActor* HitActor = Hit.GetActor(); // or whatever you're using
	if (Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), MeleeDamage, GetInstigator()->GetController(), this,  UDamageType::StaticClass());

		if (HitActor->GetInstigator() == GetInstigator())
		{
			return;
		}
		
		if (IHitInterface* HitInterface = Cast<IHitInterface>(Hit.GetActor()))
		{
			if (HitInterface)
			{
				HitInterface->Execute_GetHit(Hit.GetActor(), Hit.ImpactPoint);
			}
			IgnoreActors.AddUnique(Hit.GetActor());

			CreateFields(Hit.ImpactPoint);

		}
		// DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 10, FColor::Red, false, 0.1f, 0, 10.f);
	}
}

void AMeleeWeapon::TickAttackTrace()
{
	TraceBetweenPoints(LastTraceLocationHilt, TracePointHilt);
	TraceBetweenPoints(LastTraceLocationMid, TracePointMid);
	TraceBetweenPoints(LastTraceLocationTip, TracePointTip);

}

void AMeleeWeapon::EndAttack()
{
	// UE_LOG(LogTemp, Warning, TEXT("☑️ Melee Trace Ended"));

	bIsTracing = false;

	LastTraceLocationTip = FVector::ZeroVector;
	LastTraceLocationMid = FVector::ZeroVector;
	LastTraceLocationHilt = FVector::ZeroVector;

	IgnoreActors.Empty();

	// ✅ Allow player to attack again
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (AFillainCharacter* FillainChar = Cast<AFillainCharacter>(OwnerCharacter))
		{
			FillainChar->Combat->ActionState = EActionState::EAS_Unoccupied;
			// UE_LOG(LogTemp, Warning, TEXT("🎮 ActionState set to Unoccupied"));
		}
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