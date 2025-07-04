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
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Enemies/EnemyBase.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(WeaponMesh, TEXT("RootSocket"));
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetBoxExtent(FVector(10.f, 50.f, 50.f)); // exaggerate to test
	
	TracePointHilt = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointHilt"));
	TracePointHilt->SetupAttachment(WeaponBox);

	TracePointMid = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointMid"));
	TracePointMid->SetupAttachment(WeaponBox);

	TracePointTip = CreateDefaultSubobject<USceneComponent>(TEXT("TracePointTip"));
	TracePointTip->SetupAttachment(WeaponBox);
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	DrawDebugBox(GetWorld(), WeaponBox->GetComponentLocation(), WeaponBox->GetScaledBoxExtent(), WeaponBox->GetComponentQuat(), FColor::Red, false, 3.f);
    UE_LOG(LogTemp, Warning, TEXT("WeaponBox Rotation at BeginPlay: %s"), *WeaponBox->GetComponentRotation().ToString());
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
	Super::Equip(InParent, InSocketName, NewOwner, NewInstigator);

	// Set melee-specific collision responses
	if (GetOwner()->IsA(AFillainCharacter::StaticClass()))
	{
		WeaponBox->SetCollisionObjectType(ECC_PCWeaponBox);
		WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		WeaponBox->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	}

	// ✅ Disable collision until attack
	if (WeaponBox)
	{
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void AMeleeWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	Super::AttachMeshToSocket(InParent, InSocketName);
	
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
	UE_LOG(LogTemp, Warning, TEXT("BeginAttack — Tracing Started. Time: %f"), GetWorld()->GetTimeSeconds());
	bIsTracing = true;

	IgnoreActors.Empty();

	for (AActor* Ignored : IgnoreActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("🛑 Ignoring: %s"), *Ignored->GetName());
	}
	
	LastTraceLocationTip  = TracePointTip->GetComponentLocation();
	LastTraceLocationMid  = TracePointMid->GetComponentLocation();
	LastTraceLocationHilt = TracePointHilt->GetComponentLocation();

	IgnoreActors.Empty();
	for (AActor* Ignored : IgnoreActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("🛑 Ignoring: %s"), *Ignored->GetName());
	}
	// UE_LOG(LogTemp, Warning, TEXT("🗡 Melee Trace Started"));
}

void AMeleeWeapon::ImplementLineTraceGetHit(FHitResult Hit)
{
	if (AFillainCharacter* HitChar = Cast<AFillainCharacter>(Hit.GetActor()))
	{
		HitChar->CachedDamageAmount = MeleeDamage;
		HitChar->CachedDamageEvent = FDamageEvent(UDamageType::StaticClass());
		HitChar->CachedEventInstigator = GetInstigator()->GetController();
		HitChar->CachedDamageCauser = this;
	}
	if (IHitInterface* HitInterface = Cast<IHitInterface>(Hit.GetActor()))
	{
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(Hit.GetActor(), Hit.ImpactPoint);
		}
	}
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
	
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitActor Name: %s"), *HitActor->GetName());
	}
	
	if (Hit.GetActor())
	{
		if (AFillainCharacter* HitCharacter = Cast<AFillainCharacter>(HitActor))
		{
			HitCharacter->CachedDamageAmount = MeleeDamage;
			HitCharacter->CachedDamageEvent = FDamageEvent(UDamageType::StaticClass());
			HitCharacter->CachedEventInstigator = GetInstigator()->GetController();
			HitCharacter->CachedDamageCauser = this;

			HitCharacter->Execute_GetHit(HitCharacter, Hit.ImpactPoint);
		}
		if (AEnemyBase* HitEnemy = Cast<AEnemyBase>(HitActor))
		{
			HitEnemy->CachedDamageAmount = MeleeDamage;
			HitEnemy->CachedDamageEvent = FDamageEvent(UDamageType::StaticClass());
			HitEnemy->CachedEventInstigator = GetInstigator()->GetController();
			HitEnemy->CachedDamageCauser = this;
		}
		if (HitActor->GetInstigator() == GetInstigator())
		{
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("🎯 Melee hit %s | Instigator: %s | Owner: %s"),
		*GetNameSafe(HitActor),
		*GetNameSafe(GetInstigator()),
		*GetNameSafe(GetOwner()));
		
		ImplementLineTraceGetHit(Hit);
		IgnoreActors.AddUnique(Hit.GetActor());

		CreateFields(Hit.ImpactPoint);

		
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
		else if (AEnemyBase* EnemyOwner = Cast<AEnemyBase>(GetOwner()))
		{
			EnemyOwner->EnemyState = EEnemyState::EES_Patrolling;
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