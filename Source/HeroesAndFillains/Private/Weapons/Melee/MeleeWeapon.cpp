// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/MeleeWeapon.h"

#include "Characters/FillainCharacter.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "UI/ItemInfoWidgetBase.h"
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
#include "Components/SphereComponent.h"
#include "UI/ItemInfoWidgetBase.h" //
#include "UI/PickupGearWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Melee/StormWeapons.h"

AMeleeWeapon::AMeleeWeapon()
	: Super()
{
		PrimaryActorTick.bCanEverTick = false;
	// ✅ Then create and attach the mesh
	MeleeWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(MeleeWeaponMesh);
	
	MeleeWeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeleeWeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Reset transforms
	MeleeWeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	MeleeWeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
	MeleeWeaponMesh->SetRelativeScale3D(InitialMeshScale);
	MeleeWeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE); // Set a custom depth stencil value for the mesh
	MeleeWeaponMesh->MarkRenderStateDirty(); // Mark the render state as dirty to ensure the custom depth is applied
	EnableCustomDepth(true); // Enable custom depth rendering for the mesh

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereCollision->SetupAttachment(MeleeWeaponMesh);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionObjectType(ECC_Pickupable);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	check(SphereComp);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionObjectType(ECC_Pickupable);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupGearWidgetComponent"));
	PickupWidgetComponent->SetupAttachment(MeleeWeaponMesh);
	PickupWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PickupWidgetComponent->SetGenerateOverlapEvents(true);
	PickupWidgetComponent->SetVisibility(false);
	PickupWidgetComponent->SetWidgetClass(PickupWidgetClass);
	PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	PickupWidgetComponent->SetDrawSize(FVector2D(300.f, 50.f));
	PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, -10.f)); // below the we
	PickupWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	PickupWidgetComponent->SetDrawAtDesiredSize(true);
	PickupWidgetComponent->SetTickWhenOffscreen(true);
	PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
	PickupWidgetComponent->TranslucencySortPriority = 5;
	PickupWidgetComponent->SetVisibility(false);
	
	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
	InfoWidgetComponent->SetupAttachment(MeleeWeaponMesh);
	InfoWidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InfoWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	InfoWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InfoWidgetComponent->SetGenerateOverlapEvents(true);
	InfoWidgetComponent->SetVisibility(false);
	InfoWidgetComponent->SetWidgetClass(InfoWidgetClass);
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
	InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // above the weapon
	InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	InfoWidgetComponent->SetDrawAtDesiredSize(true);
	InfoWidgetComponent->SetTickWhenOffscreen(true);
	InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
	InfoWidgetComponent->TranslucencySortPriority = 5;
	InfoWidgetComponent->SetVisibility(false);

	HoveringLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("HoveringLight"));
	HoveringLight->SetupAttachment(MeleeWeaponMesh);

	// Settings
	HoveringLight->SetIntensity(2000.f);  // How bright
	HoveringLight->SetAttenuationRadius(300.f); // How far it shines
	HoveringLight->SetLightColor(FLinearColor(0.f, 0.5f, 1.f)); // Slight blue glow
	HoveringLight->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Glow slightly under rifle
	HoveringLight->bUseInverseSquaredFalloff = false; // Makes intensity easier to control
	HoveringLight->SetVisibility(true);

	HoveringDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("HoveringDecal"));
	HoveringDecal->SetupAttachment(MeleeWeaponMesh);
	HoveringDecal->DecalSize = FVector(64.f, 128.f, 128.f); // Flat and wide
	HoveringDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // Face it downward
	HoveringDecal->SetRelativeLocation(FVector(0.f, 0.f, -55.f)); // Slightly under rifle
	HoveringDecal->SetVisibility(true);
	
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

	if (IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // or World
		InfoWidgetComponent->SetDrawAtDesiredSize(true);
		if (InfoWidgetClass)
		{
			InfoWidgetComponent->SetWidgetClass(InfoWidgetClass);
		}
	}
}



void AMeleeWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	// Set up attachment rules
	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale
		true);

	// Perform the attachment
	MeleeWeaponMesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (bIsEquipped == false)
	{
		SetMeleeWeaponInformationText(GetInfoWidgetComponent(), this);
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

	if (AStormWeapons* StormWeapon = Cast<AStormWeapons>(this)) return;
	
	if (PickupWidgetComponent)
	{
		PickupWidgetComponent->UpdateComponentToWorld();
	}
	if (InfoWidgetComponent)
	{
		InfoWidgetComponent->UpdateComponentToWorld();
	}
	if (!PickupWidgetComponent)
	{
		PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponent"));
		if (PickupWidgetComponent)
		{
			PickupWidgetComponent->RegisterComponent();
			PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupWidgetComponent->InitWidget();
			PickupWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			PickupWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			PickupWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			PickupWidgetComponent->SetDrawAtDesiredSize(true);
			PickupWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			PickupWidgetComponent->SetTickWhenOffscreen(true);
			PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupWidgetComponent->TranslucencySortPriority = 5;
			if (auto* W = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject()))
			{
				W->PickupGearOwningComponent = PickupWidgetComponent;
			}
		}
	}
	if (!InfoWidgetComponent)
	{
		InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("InfoWidgetComponentA"));
		if (InfoWidgetComponent)
		{
			InfoWidgetComponent->RegisterComponent();
			InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			InfoWidgetComponent->InitWidget();
			InfoWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetCollisionObjectType(ECC_Mesh);
			PickupWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			PickupWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			InfoWidgetComponent->SetDrawAtDesiredSize(true);
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			InfoWidgetComponent->SetTickWhenOffscreen(true);
			InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			InfoWidgetComponent->TranslucencySortPriority = 5;
			if (auto* IIW = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject()))
			{
				IIW->ItemInfoOwningComponent = InfoWidgetComponent;
			}
		}
	}
	if (InfoWidgetComponent)
	{
		if (AActor* PickupItemActor = Cast<AActor>(InfoWidgetComponent->GetOwner()))
		{
			if (AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(PickupItemActor)) MeleeWeapon->SetMeleeWeaponInformationText(InfoWidgetComponent, MeleeWeapon);
			else if (ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(PickupItemActor)) RangedWeapon->SetRangedWeaponInformationText(InfoWidgetComponent, RangedWeapon);
			else if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(PickupItemActor)) AmmoPickup->SetAmmoPickupInformationText(InfoWidgetComponent, AmmoPickup);
			else return;
		}
	}
}

void AMeleeWeapon::DisableSphereCollision()
{
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMeleeWeapon::EnableCustomDepth(bool bEnable)
{
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetRenderCustomDepth(bEnable);
	}
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AMeleeWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	if (!InParent) return;
    
	// Make sure we're attaching to the skeletal mesh
	if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(InParent))
	{
		if (!SkeletalMesh->DoesSocketExist(InSocketName)) return;
		if (NewOwner->ActorHasTag(FName("Player")))
		{
			Character = Cast<AFillainCharacter>(NewOwner);
			if (Character && Character->GetMesh())
			{
				// Just call Super::Equip - it will handle the attachment
			}
		}
		else if (NewOwner->ActorHasTag(FName("Enemy")))
		{
			if (AEnemyBase* EnChar = Cast<AEnemyBase>(NewOwner))
			{
				if (EnChar->GetMesh())
				{
					// Just call Super::Equip - it will handle the attachment
				}
			}
		}
	}

	// ---- Collision setup (after attach & owner set) ----
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
	if (NewOwner->IsA(AFillainCharacter::StaticClass()))
	{
		WeaponBox->SetCollisionObjectType(ECC_PCWeaponBox);
		WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		WeaponBox->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
		WeaponBox->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECR_Overlap);
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (NewOwner->IsA(AEnemyBase::StaticClass()))
	{
		WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
		WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
		WeaponBox->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
		MeleeWeaponMesh->AttachToComponent(InParent, TransformRules, InSocketName);
		ItemState = EItemState::EIS_Equipped;
		bIsEquipped = true;
		SetOwner(NewOwner);
		ShowPickupAndInfoWidgets(false);
		SetInstigator(NewInstigator);
		AttachMeshToSocket(InParent, InSocketName);
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
		MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AActor* OwnerCharacter = GetOwner(); // Typically set on equip
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this); // Ignore the weapon itself
		if (OwnerCharacter) TraceParams.AddIgnoredActor(OwnerCharacter); // ✅ Ignore the wielder!
		// UE_LOG(LogTemp, Warning, TEXT("Attaching %s to %s at socket %s"), *GetName(), *InParent->GetName(), *InSocketName.ToString());
		// UE_LOG(LogTemp, Warning, TEXT("Post-Attach Location: %s"), *GetActorLocation().ToString());
	}
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

		// Damage is now handled through GAS via GetHit_Implementation
		// which will apply the appropriate GameplayEffect
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
	// Damage is now handled through GAS in GetHit_Implementation
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
        true
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
	    		// Damage is now handled through GAS in GetHit_Implementation
	    		HitBase->Execute_GetHit(HitBase, Hit.ImpactPoint, GetOwner());
	    		ImplementLineTraceGetHit(Hit);

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

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PickupWidgetComponent && InfoWidgetComponent)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->PlayerCameraManager->GetCameraLocation());
			PickupWidgetComponent->SetWorldRotation(LookRotation);
			InfoWidgetComponent->SetWorldRotation(LookRotation);
		}
	}
	
	if (bShouldHover)
	{
		float RunTime = GetGameTimeSinceCreation(); // How long this actor has existed
		float DeltaHeight = FMath::Sin(RunTime * HoverSpeed) * HoverAmplitude * DeltaTime;

		FVector NewLocation = GetActorLocation();
		NewLocation.Z += DeltaHeight;
		SetActorLocation(NewLocation);
	}

	if (bShouldFloatSpin) // You can make a bool for it
	{
		AddActorLocalRotation(FRotator(0.f, 30.f * DeltaTime, 0.f)); // 30 degrees per second
	}
	
	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
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
			if (!PickupWidgetComponent && !IsValid(PickupWidgetComponent))
			{
				PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
				if (PickupWidgetComponent)
				{
					PickupWidgetComponent->RegisterComponent();
					PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
					PickupWidgetComponent->InitWidget();
					PickupWidgetComponent->SetVisibility(true);
					PickupWidgetComponent->SetTickWhenOffscreen(true);
					PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
					PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
					PickupWidgetComponent->TranslucencySortPriority = 5;
				}
				UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject());
				if (PickupWidgetA)
				{
					PickupWidgetComponent = PickupWidgetA->GetOwningWidgetComponent();
				}
			}
			if (!IsValid(this)) return;
    
			if (!InfoWidgetComponent && !IsValid(InfoWidgetComponent))
			{
				InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("InfoWidgetComponentA"));
				if (InfoWidgetComponent)
				{
					InfoWidgetComponent->RegisterComponent();
					InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
					InfoWidgetComponent->InitWidget();
					InfoWidgetComponent->SetVisibility(true);
					InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
					InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
					InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
					InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
					InfoWidgetComponent->SetDrawAtDesiredSize(true);
					InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
					InfoWidgetComponent->SetVisibility(true);
					InfoWidgetComponent->SetTickWhenOffscreen(true);
					InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
					InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
					InfoWidgetComponent->TranslucencySortPriority = 5;
				}
				UItemInfoWidgetBase* IWidget = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject());
				if (IWidget)
				{
					InfoWidgetComponent = IWidget->GetItemInfoOwningComponent();
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

void AMeleeWeapon::OnEquippedOneHanded()
{
	MeleeWeaponMesh->SetSimulatePhysics(false);
	MeleeWeaponMesh->SetEnableGravity(false);
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = false;
			bShouldFloatSpin = false;
			ShowPickupAndInfoWidgets(false);
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AMeleeWeapon::OnEquippedTwoHanded()
{

	if (MeleeWeaponMesh)
	{
		// Create and set up a socket for the left hand if it doesn't exist
		if (!MeleeWeaponMesh->DoesSocketExist(FName("LeftHandSocket")))
		{
			// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket does not exist on weapon mesh"));
			return;
		}

		// Get the socket transform
		FTransform LeftHandSocketTransform = MeleeWeaponMesh->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
        
		// Log the socket location for debugging
		// UE_LOG(LogTemp, Warning, TEXT("LeftHandSocket transform - Location: %s, Rotation: %s"), 
		//			*LeftHandSocketTransform.GetLocation().ToString(),
		//			*LeftHandSocketTransform.GetRotation().Rotator().ToString());;
		
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeleeWeaponMesh->SetSimulatePhysics(false);
		MeleeWeaponMesh->SetEnableGravity(false);
		MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Delay visual effects
		GetWorld()->GetTimerManager().SetTimer
		(VisualEffectsTimerHandle,
			[this]()
			{
				bShouldHover = false;
				bShouldFloatSpin = false;
				ShowPickupAndInfoWidgets(false);
				SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AMeleeWeapon::OnDropped()
{
	MeleeWeaponMesh->SetSimulatePhysics(true);
	MeleeWeaponMesh->SetEnableGravity(true);
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (!SphereCollision)
	{
		SphereCollision = NewObject<USphereComponent>(this, TEXT("Area SphereCollision"));
		SphereCollision->RegisterComponent();
		SphereCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	if (HasAuthority())
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	}
	MeleeWeaponMesh->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
	MeleeWeaponMesh->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
	MeleeWeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Delay visual effects
	GetWorld()->GetTimerManager().SetTimer
	(VisualEffectsTimerHandle,
		[this]()
		{
			bShouldHover = true;
			bShouldFloatSpin = true;
			MeleeWeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
			MeleeWeaponMesh->MarkRenderStateDirty();
			EnableCustomDepth(true);
	
			HoveringDecal->SetVisibility(true);
			HoveringLight->SetVisibility(true);
		},
		0.1f,
		false);
	
	if (!IsValid(this)) return;
    
	if (!PickupWidgetComponent && !IsValid(PickupWidgetComponent))
	{
		PickupWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("PickupWidgetComponentA"));
		if (PickupWidgetComponent)
		{
			PickupWidgetComponent->RegisterComponent();
			PickupWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupWidgetComponent->SetWidgetClass(UPickupGearWidget::StaticClass());
			PickupWidgetComponent->InitWidget();
			PickupWidgetComponent->SetVisibility(true);
			PickupWidgetComponent->SetTickWhenOffscreen(true);
			PickupWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			PickupWidgetComponent->SetWorldScale3D(FVector(1.0f));
			PickupWidgetComponent->TranslucencySortPriority = 5;
		}
		UPickupGearWidget* PickupWidgetA = Cast<UPickupGearWidget>(PickupWidgetComponent->GetUserWidgetObject());
		if (PickupWidgetA)
		{
			PickupWidgetComponent = PickupWidgetA->GetOwningWidgetComponent();
		}
	}
	if (!IsValid(this)) return;
    
	if (!InfoWidgetComponent && !IsValid(InfoWidgetComponent))
	{
		InfoWidgetComponent = NewObject<UWidgetComponent>(this, TEXT("InfoWidgetComponentA"));
		if (InfoWidgetComponent)
		{
			InfoWidgetComponent->RegisterComponent();
			InfoWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			InfoWidgetComponent->SetWidgetClass(UItemInfoWidgetBase::StaticClass());
			InfoWidgetComponent->InitWidget();
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
			InfoWidgetComponent->SetDrawSize(FVector2D(300.f, 200.f));
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // above the weapon
			InfoWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
			InfoWidgetComponent->SetDrawAtDesiredSize(true);
			InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
			InfoWidgetComponent->SetVisibility(true);
			InfoWidgetComponent->SetTickWhenOffscreen(true);
			InfoWidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));
			InfoWidgetComponent->SetWorldScale3D(FVector(1.0f));
			InfoWidgetComponent->TranslucencySortPriority = 5;
		}
		UItemInfoWidgetBase* FoWidget = Cast<UItemInfoWidgetBase>(InfoWidgetComponent->GetUserWidgetObject());
		if (FoWidget)
		{
			InfoWidgetComponent = FoWidget->GetItemInfoOwningComponent();
		}
	}
}

void AMeleeWeapon::OnEquippedSecondary()
{
	bShouldHover = false;
	bShouldFloatSpin = false;
	ShowPickupAndInfoWidgets(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->DestroyComponent(); SphereCollision = nullptr;
	MeleeWeaponMesh->SetSimulatePhysics(false);
	MeleeWeaponMesh->SetEnableGravity(false);
	MeleeWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
	HoveringDecal->SetVisibility(false);
	HoveringLight->SetVisibility(false);

	// Safely destroy existing widgets
	if (PickupWidgetComponent) { PickupWidgetComponent->DestroyComponent(); PickupWidgetComponent = nullptr; }
	if (InfoWidgetComponent) { InfoWidgetComponent->DestroyComponent(); InfoWidgetComponent = nullptr; }
}

void AMeleeWeapon::WeaponDropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	MeleeWeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	FillainOwnerCharacter = nullptr;
	FillainOwnerController = nullptr;
}

void AMeleeWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_EquippedOneHanded:
		OnEquippedOneHanded();
		break;
	case EWeaponState::EWS_EquippedTwoHanded:
		OnEquippedTwoHanded();
		break;
	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}
}

void AMeleeWeapon::SetEquippedWeaponState()
{
	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher || WeaponType == EWeaponType::EWT_SniperRifle || WeaponType == EWeaponType::EWT_Shotgun || WeaponType == EWeaponType::EWT_ChaosSword)
	{
		WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		OnWeaponStateSet();
	}
	if (WeaponType == EWeaponType::EWT_AssaultRifle || WeaponType == EWeaponType::EWT_SubmachineGun || WeaponType == EWeaponType::EWT_Pistol || WeaponType == EWeaponType::EWT_RubySword || WeaponType == EWeaponType:: EWT_SapphireSword || WeaponType == EWeaponType:: EWT_SandSword || WeaponType == EWeaponType:: EWT_SoulSword || WeaponType == EWeaponType:: EWT_ShadowSword || WeaponType == EWeaponType:: EWT_SkyMace)
	{
		WeaponState = EWeaponState::EWS_EquippedOneHanded;
		OnWeaponStateSet();
	}
}

void AMeleeWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}