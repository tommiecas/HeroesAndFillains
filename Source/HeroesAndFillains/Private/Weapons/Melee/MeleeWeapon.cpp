// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "Characters/FillainCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Enemies/EnemyBase.h"
#include "DrawDebugHelpers.h"
#include "HAFComponents/CombatComponent.h" // if you want to pull from there

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(WeaponMesh, TEXT("RootSocket"));
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionObjectType(ECC_PCWeaponBox);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
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
	
	/* DrawDebugBox(GetWorld(), WeaponBox->GetComponentLocation(), WeaponBox->GetScaledBoxExtent(), WeaponBox->GetComponentQuat(), FColor::Red, false, 3.f); */
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
		WeaponBox->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECR_Overlap);
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
	bIsTracing = true;
	IgnoreActors.Empty();

	// Cache Vision pad from the owner (server does the authoritative traces)
	CachedVisionPadCM = 0.f;
	if (AFillainCharacter* FC = Cast<AFillainCharacter>(GetOwner()))
	{
		// If you implemented CurrentHitAssistPaddingCM on the combat component:
		if (FC->Combat)
		{
			// Optionally: FC->Combat->ServerCacheHitAssistPadding(); // if you RPC’d earlier
			CachedVisionPadCM = FC->Combat->CurrentHitAssistPaddingCM; // replicated
		}
		else
		{
			// Fallback: compute directly
			CachedVisionPadCM = FC->GetHitAssistPaddingCM();
		}
	}

	LastTraceLocationTip  = TracePointTip->GetComponentLocation();
	LastTraceLocationMid  = TracePointMid->GetComponentLocation();
	LastTraceLocationHilt = TracePointHilt->GetComponentLocation();
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
			HitInterface->Execute_GetHit(Hit.GetActor(), Hit.ImpactPoint, GetOwner());
		}
	}
}

void AMeleeWeapon::TraceBetweenPoints(FVector& LastLocation, USceneComponent* TracePoint)
{
	 const FVector CurrentLocation = TracePoint->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore = { this, GetOwner() };
    for (AActor* Actor : IgnoreActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }

    // Radius = base blade thickness + Vision pad
    const float TraceRadius = FMath::Clamp(BaseTraceRadiusCM + CachedVisionPadCM, 0.f, 30.f);

    // Use the same trace channel you already used (TraceTypeQuery1).
    // If that channel hits world AND characters, consider switching this to “ForObjects” and only include Pawns.
    TArray<FHitResult> Hits;
    const bool bHitAny = UKismetSystemLibrary::SphereTraceMulti(
        this,
        LastLocation,
        CurrentLocation,
        TraceRadius,
        ETraceTypeQuery::TraceTypeQuery1,
        /*bTraceComplex=*/ false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        Hits,
        /*bIgnoreSelf=*/ true
    );

    if (bHitAny)
    {
        for (const FHitResult& Hit : Hits)
        {
            AActor* HitActor = Hit.GetActor();
            if (!HitActor) continue;

            // Skip self/owner just in case
            if (HitActor == this || HitActor == GetOwner()) continue;

            // Don’t multi-hit same actor this swing
            if (IgnoreActors.Contains(HitActor)) continue;

            // Only process characters (avoid world clutter)
            if (ABaseCharacter* HitBase = Cast<ABaseCharacter>(HitActor))
            {
                HitBase->CachedDamageAmount      = MeleeDamage;
                HitBase->CachedDamageEvent       = FDamageEvent(UDamageType::StaticClass());
                HitBase->CachedEventInstigator   = GetInstigator()->GetController();
                HitBase->CachedDamageCauser      = this;

                HitBase->Execute_GetHit(HitBase, Hit.ImpactPoint, GetOwner());
                ImplementLineTraceGetHit(Hit); // your extra handling is fine

                IgnoreActors.AddUnique(HitActor);
                CreateFields(Hit.ImpactPoint);
            }
        }
    }

#if !(UE_BUILD_SHIPPING)
    // Visualize the Vision-assisted sweep for debugging
    const FVector Mid = (LastLocation + CurrentLocation) * 0.5f;
    const FVector Delta = CurrentLocation - LastLocation;
    const float HalfLen = Delta.Size() * 0.5f;
    const FQuat Rot = FRotationMatrix::MakeFromZ(Delta.GetSafeNormal()).ToQuat();
    DrawDebugCapsule(GetWorld(), Mid, HalfLen, TraceRadius, Rot, FColor::Cyan, false, 0.05f, 0, 0.75f);
#endif

    // Advance
    LastLocation = CurrentLocation;
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