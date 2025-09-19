// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "Characters/FillainCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "Weapons/WeaponBase.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Enemies/EnemyBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HAFComponents/CombatComponent.h" // if you want to pull from there
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "HUD/ItemInfoWidgetBase.h" //
#include "HUD/PickupGearWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Melee/StormWeapons.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
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

	if (IsValid(ItemInfoWidgetComponent))
	{
		ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // or World
		ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
		if (ItemInfoWidgetClass)
		{
			ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
		}
	}
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	Super::EnableCustomDepth(bEnable);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (bIsEquipped == false)
	{
		SetMeleeWeaponInformationText(GetItemInfoWidgetComponent(), this);
	}
	if (bIsEquipped == true)
	{
		SetEquippedMeleeWeaponState();
		if (WeaponState == EWeaponState::EWS_EquippedOneHanded) OnEquippedOneHanded();
		if (WeaponState == EWeaponState::EWS_EquippedTwoHanded) OnEquippedTwoHanded();
		if (WeaponState == EWeaponState::EWS_EquippedSecondary) OnEquippedSecondary();
		if (WeaponState == EWeaponState::EWS_Dropped) OnDropped();
		if (WeaponState == EWeaponState::EWS_Unclaimed) return;
	}
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnBoxOverlap);
}

bool AMeleeWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AMeleeWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);	
	}
}

void AMeleeWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AMeleeWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
	this,
	Start,
	End,
	BoxTraceExtent,
	BoxTraceStart->GetComponentRotation(),
	ETraceTypeQuery::TraceTypeQuery1,
	false,
	ActorsToIgnore,
	bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	BoxHit,
	true
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AMeleeWeapon::OnEquippedOneHanded()
{
	Super::OnEquippedOneHanded();
	
}
void AMeleeWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    ShowPickupAndInfoWidgets(false);
    PlayEquipSound();
    DisableSphereCollision();
    SetHandsNeeded(this);
    SetEquippedWeaponState();
    if (HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon) OnEquippedOneHanded();
    if (HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon) OnEquippedTwoHanded();
    SetOneOrTwoHandedWeapon(this);
    DeactivateEmbers();
    bShouldHover = false;
    bShouldFloatSpin = false;

    // ---- Attach using the validated skeletal mesh + socket ----
    {
        // If your Super::Equip didn’t attach, do it here; otherwise, remove one of them.
        const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		AFillainCharacter* FillCharacter = Cast<AFillainCharacter>(GetOwner());
        if (FillCharacter) AttachToComponent(FillCharacter->GetMesh(), Rules, InSocketName);
    }

    // Optional: if you really want to use USkeletalMeshSocket API, do NOT pull from Character->GetMesh().
    // Use the SAME parent we've validated above:
    // if (const USkeletalMeshSocket* Socket = SkelParent->GetSocketByName(InSocketName)) {
    //     Socket->AttachActor(this, SkelParent);
    // }

    // ---- Collision setup (after attach & owner set) ----
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (AActor* OwnerActor = GetOwner())
    {
        if (OwnerActor->IsA(AFillainCharacter::StaticClass()))
        {
            WeaponBox->SetCollisionObjectType(ECC_PCWeaponBox);
            WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
            WeaponBox->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
            WeaponBox->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECR_Overlap);
            WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        else if (OwnerActor->IsA(AEnemyBase::StaticClass()))
        {
            WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
            WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
            WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
            WeaponBox->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
            WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }

    // (Your trace params setup can stay, but it’s unrelated to Equip and can move elsewhere.)
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
		// If you implemented CurrentHitAssistPaddingCM on the CombatComponent component:
		if (FC->CombatComponent)
		{
			// Optionally: FC->CombatComponent->ServerCacheHitAssistPadding(); // if you RPC’d earlier
			CachedVisionPadCM = FC->CombatComponent->CurrentHitAssistPaddingCM; // replicated
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
			FillainChar->CombatComponent->ActionState = EActionState::EAS_Unoccupied;
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

void AMeleeWeapon::SetMeleeWeaponInformationText(UWidgetComponent* MeleeItemInfoComp, AMeleeWeapon* MeleeWeapon)
{
	if (AStormWeapons* StormW = Cast<AStormWeapons>(this))
	{
		if (StormW->bIsEquipped == true)
		{
			return;
		}
		if (StormW->bIsEquipped == false)
		{
			if (!PickupGearWidgetComponent && !IsValid(PickupGearWidgetComponent))
			{
				PickupGearWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
				if (PickupGearWidgetComponent)
				{
					PickupGearWidgetComponent->RegisterComponent();
					PickupGearWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					PickupGearWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
					PickupGearWidgetComponent->InitWidget();
					PickupGearWidgetComponent->SetVisibility(true);
					PickupGearWidgetComponent->SetTickWhenOffscreen(true);
					PickupGearWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
					PickupGearWidgetComponent->SetWorldScale3D(FVector(1.0f));
					PickupGearWidgetComponent->TranslucencySortPriority = 5;
				}
				UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupGearWidgetComponent->GetUserWidgetObject());
				if (PickupWidgetA)
				{
					PickupGearWidgetComponent = PickupWidgetA->GetOwningWidgetComponent();
				}
			}
			if (!IsValid(this)) return;
    
			if (!ItemInfoWidgetComponent && !IsValid(ItemInfoWidgetComponent))
			{
				ItemInfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("ItemInfoWidgetComponentA"));
				if (ItemInfoWidgetComponent)
				{
					ItemInfoWidgetComponent->RegisterComponent();
					ItemInfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					ItemInfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
					ItemInfoWidgetComponent->InitWidget();
					ItemInfoWidgetComponent->SetVisibility(true);
					ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
					ItemInfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
					ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
					ItemInfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
					ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
					ItemInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
					ItemInfoWidgetComponent->SetVisibility(true);
					ItemInfoWidgetComponent->SetTickWhenOffscreen(true);
					ItemInfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
					ItemInfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
					ItemInfoWidgetComponent->TranslucencySortPriority = 5;
				}
				UItemInfoWidgetBase* InfoWidget = Cast<UItemInfoWidgetBase>(ItemInfoWidgetComponent->GetUserWidgetObject());
				if (InfoWidget)
				{
					ItemInfoWidgetComponent = InfoWidget->GetItemInfoOwningComponent();
				}
			}
		}
	}
	if (!IsValid(MeleeItemInfoComp))
	{	
		UE_LOG(LogTemp, Warning, TEXT("SetMeleeWeaponInformationText: WidgetComp is invalid."));
		return;
	}
	UUserWidget* RawWidget = MeleeItemInfoComp->GetUserWidgetObject();
	if (!IsValid(RawWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetMeleeWeaponInformationText: Widget instance missing after InitWidget on %s (class: %s)."),
			*GetNameSafe(MeleeItemInfoComp),
			*GetNameSafe(MeleeItemInfoComp->GetWidgetClass()));
			return;
	}
	if (UItemInfoWidgetBase* Info = Cast<UItemInfoWidgetBase>(RawWidget))
	{
		// Replace these with your actual properties / setters
		Info->SetMeleeInfo(MeleeWeaponName, MeleeWeaponHistory, MeleeWeaponResistances, MeleeWeaponWeaknesses, MeleeWeaponDamage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetMeleeWeaponInformationText: Expected UMeleeWeaponInfoWidget, got %s on %s."),
			*GetNameSafe(RawWidget->GetClass()),
			*GetNameSafe(MeleeItemInfoComp));
	}
}