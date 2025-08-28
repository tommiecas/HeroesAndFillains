// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/FillainCharacter.h"  
#include "GameFramework/SpringArmComponent.h"  
#include "Camera/CameraComponent.h"  
#include "GameFramework/CharacterMovementComponent.h"  
#include "Components/InputComponent.h"  
#include "EnhancedInputSubsystems.h"  
#include "EnhancedInputComponent.h"  
#include "Components/WidgetComponent.h"  
#include "Net/UnrealNetwork.h"  
#include "Weapons/WeaponBase.h"
#include "HAFComponents/CombatComponent.h"  
#include "HAFComponents/BuffComponent.h"  
#include "Components/CapsuleComponent.h"  
#include <Kismet/KismetMathLibrary.h>  
#include "Characters/FillainAnimInstance.h"  
#include "HeroesAndFillains/HeroesAndFillains.h"  
#include "PlayerController/FillainPlayerController.h"  
#include "GameMode/HAFGameMode.h"  
#include "TimerManager.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"  
#include "Particles/ParticleSystemComponent.h"  
#include "PlayerState/HAFPlayerState.h"  
#include "Weapons/WeaponTypes.h"  
#include "Components/BoxComponent.h"  
#include "HAFComponents/LagCompensationComponent.h"  
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "GameStates/HAFGameState.h"  
#include "PlayerStart/TeamPlayerStart.h"
#include "GameFramework/Actor.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Pickups/AmmoPickup.h"
#include "Components/StaticMeshComponent.h"
#include "HAFComponents/AttributeComponent.h"
#include "Items/Treasure.h"
#include "Items/Soul.h"
#include "AbilitySystemComponent.h"
#include "InputCoreTypes.h"
#include "InputActionValue.h"
#include "InputTriggers.h"
#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Components/InputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "TimerManager.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Enemies/EnemyBase.h"
#include "Pickups/HealthPickup.h"
#include "Pickups/MajixPickup.h"
#include "Pickups/ShieldPickup.h"
#include "Pickups/StaminaPickup.h"
#include "HAFComponents/HiddenTreasureComponent.h"
#include "HAFComponents/HiddenTreasureScannerComponent.h"
#include "HeroesAndFillains/DebugMacros.h"
#include "Input/HAFInputComponent.h"
#include "Misc/OutputDevice.h"
#include "Misc/AssertionMacros.h"
#include "HAL/PlatformStackWalk.h"
#include "Logging/LogMacros.h"
#include "HAL/PlatformStackWalk.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/AssertionMacros.h"        // FDebug::DumpStackTraceToLog
#include "Misc/EngineVersionComparison.h"
#include "HAL/PlatformStackWalk.h"
#include "Logging/LogMacros.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/LightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/LightComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"

void AFillainCharacter::Client_DisableAllWorldPPV_Implementation()
{
	int32 Count = 0;

	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		APostProcessVolume* V = *It;
		if (!IsValid(V)) continue;

		// Hard-disable this volume
		V->bUnbound     = false;
		V->BlendWeight  = 0.f;
		V->Priority     = -9999.f;              // keep it out of blends
		V->Settings     = FPostProcessSettings(); // reset to defaults

		++Count;
		UE_LOG(LogTemp, Warning, TEXT("[PPV-Off] Disabled %s (in level: %s)"),
			   *GetNameSafe(V), *GetNameSafe(V->GetLevel()));
	}

	UE_LOG(LogTemp, Warning, TEXT("[PPV-Off] Disabled %d PostProcessVolumes"), Count);
}

void AFillainCharacter::Client_ClearCameraEffects_Implementation()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Turn OFF any PlayerController fade overlay (color must be FColor)
		PC->ClientSetCameraFade(
			/*bEnable=*/false,
			FColor(0,0,0,255),       // ← use FColor, not FLinearColor
			FVector2D(0.f, 0.f),
			/*FadeTime=*/0.f,
			/*bFadeAudio=*/false,
			/*bHoldWhenFinished=*/false
		);

		if (PC->PlayerCameraManager)
		{
			// Stop legacy/new camera shakes
			PC->PlayerCameraManager->StopAllCameraShakes(/*bImmediately=*/true);

			// Remove any lens effects (can look like solid tints/flares)
			PC->PlayerCameraManager->ClearCameraLensEffects();

			// (Optional, if available in your build) force fade alpha to 0 instantly:
			// PC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 0.f, FLinearColor::Black, false, false);
		}
	}
}


AFillainCharacter::AFillainCharacter()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerCharacter);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pickupable, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Treasure, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Area, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	
	
	PrimaryActorTick.bCanEverTick = true;
	// SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);
	
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECC_Mesh);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(50.f);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bSelfOccluded = false;
	CameraSelfOcclusionThreshold = 160.f;
	
	/***********************************************
	****    Hit boxes for server-side rewind    ****
	***********************************************/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);
	
	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AFillainCharacter::FixSelfCameraCollision()
{
	// Your pawn should never block the camera probe
	if (UCapsuleComponent* Cap = GetCapsuleComponent())
		Cap->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
		SkelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); // covers backpack bone

	// Any other primitives on the pawn (quads/widgets/etc.)
	TArray<UPrimitiveComponent*> Prims;
	GetComponents<UPrimitiveComponent>(Prims);              // <— correct API
	for (UPrimitiveComponent* P : Prims)
		if (P) P->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	if (CameraBoom)
		CameraBoom->ProbeChannel = ECC_Camera;
}

void AFillainCharacter::Debug_ProbeSpringArmBlocker()
{
	if (!CameraBoom || !FollowCamera) return;

	const FVector Pivot   = CameraBoom->GetComponentLocation();
	const FRotator Rot    = CameraBoom->GetComponentRotation();
	const FVector Desired = Pivot - Rot.Vector() * CameraBoom->TargetArmLength;

	const float Target = CameraBoom->TargetArmLength;
	const float Actual = (FollowCamera->GetComponentLocation() - Pivot).Size();

	UE_LOG(LogTemp, Warning, TEXT("[ArmDbg] Target=%.1f  Actual=%.1f  ProbeSize=%.1f  Channel=%d"),
		Target, Actual, CameraBoom->ProbeSize, (int32)CameraBoom->ProbeChannel);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ArmDbg), /*bTraceComplex=*/false, this);
	// Do NOT ignore anything; we want to catch the culprit (even if attached to us)
	bool bHit = GetWorld()->SweepSingleByChannel(
	Hit, Pivot, Desired, FQuat::Identity,
		CameraBoom->ProbeChannel,
		FCollisionShape::MakeSphere(CameraBoom->ProbeSize),
		Params);

	if (bHit && Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Error, TEXT("[ArmDbg] BLOCKED by Actor=%s  Comp=%s  Profile=%s  Dist=%.1f"),
			*GetNameSafe(Hit.GetActor()),
			*GetNameSafe(Hit.GetComponent()),
			Hit.Component.IsValid() ? *Hit.Component->GetCollisionProfileName().ToString() : TEXT("None"),
			Hit.Distance);

		// Uncomment for a visible line in PIE:
		// DrawDebugLine(GetWorld(), Pivot, Hit.ImpactPoint, FColor::Red, false, 2.f, 0, 1.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ArmDbg] No blocker found (Actual may be reduced by other code)."));
	}	
}

void AFillainCharacter::Client_OnEquipped_Implementation()
{

	bEquipInProgress = false;
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(DefaultFOV);
		bFOVLock = true;
		FOVLockTimeLeft = 0.75f; // hold for ~¾s; tweak if needed
	}
	// after the weapon attaches, owning client
	Combat->CurrentFOV = Combat->DefaultFOV;
	GetFollowCamera()->SetFieldOfView(Combat->DefaultFOV);
}

void AFillainCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("[Char] BeginPlay A"));
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[Char] BeginPlay B"));
	
	InitFillainCharacterCapsuleBaselinesIfNeeded(); 

	if (CameraBoom)
	{
		CameraBoom->bDoCollisionTest = false;   // prevents retraction
		CameraBoom->TargetArmLength  = 300.f;   // your usual 3P distance
	}
	bUseControllerRotationYaw = false;
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;
	}

	FixSelfCameraCollision();
	
	bIsCharacterDead = false;

	Combat = FindComponentByClass<UCombatComponent>();

	if (Combat)
	{
		Combat->SetCharacter(this); // this is AFillainCharacter*
	}
	
	
	if (AFillainPlayerController* FillainController = Cast<AFillainPlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(FillainController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(HAFMappingContext, 0);
		}
	}
	/*if (FillainPlayerController == nullptr)
	{
		FillainPlayerController = Cast<AFillainPlayerController>(GetController());
	}
	if (FillainPlayerController)
	{
		// Set the player state or any other necessary properties here
		FillainPlayerController->InitPlayerState();
		HAFPlayerState = FillainPlayerController->GetPlayerState<AHAFPlayerState>();

		// Log the player controller name for debugging
		UE_LOG(LogTemp, Log, TEXT("FillainPlayerController initialized: %s"), *FillainPlayerController->GetName());
	}*/

	SpawnDefaultWeapon();
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AFillainCharacter::ReceiveDamage);
	}
	HideAttachedGrenade();

	UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AFillainCharacter::OnArmDisarmMontageEnded);
	}
	
	Tags.Add(FName("EngageableTarget"));

	Tags.Add(FName("FillainCharacter"));

	Tags.Add(FName("Fillain"));

	HAFAttributes = GetHAFAttributeSet();

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), false);

	UCapsuleComponent* Cap = GetCapsuleComponent();
	check(Cap && GetMesh());
	StandingUnscaledHalfHeight = Cap->GetUnscaledCapsuleHalfHeight();
	StandingUnscaledRadius     = Cap->GetUnscaledCapsuleRadius();
	StandingScaledHalfHeight   = Cap->GetScaledCapsuleHalfHeight();
	StandingMeshRelZ           = GetMesh()->GetRelativeLocation().Z;

	// Get ASC
	ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);
	
	// Initial sync to whatever values we start with
	RequestFillainCharacterCapsuleUpdate();
		
	if (ASC && HAFAttributeSet)
	{
		// Bind to Intuition changes
		ASC->GetGameplayAttributeValueChangeDelegate(HAFAttributeSet->GetIntuitionAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
		{
			const float IntuitionFraction = Data.NewValue; // fraction (0.25 = +25%)

			// Apply to any HiddenTreasureComponent attached directly to this actor
			for (auto* HT : TInlineComponentArray<UHiddenTreasureComponent*>(this, true))
			{
				HT->ApplyIntuitionScale(IntuitionFraction);
			}
		});
	}
}

bool AFillainCharacter::IsCameraWeird(FString& OutWhy) const
{
    if (!FollowCamera || !CameraBoom || !GetCapsuleComponent()) return false;

    const float CapZ  = GetCapsuleComponent()->GetComponentLocation().Z;
    const float CamZ  = FollowCamera->GetComponentLocation().Z;
    const float dZ    = CamZ - CapZ;

    if (FollowCamera->GetAttachParent() != CameraBoom) { OutWhy = TEXT("Camera parent != CameraBoom"); return true; }
    if (dZ < -40.f)                                     { OutWhy = FString::Printf(TEXT("Camera below capsule ΔZ=%.1f"), dZ); return true; }
    if (CameraBoom->TargetArmLength < 10.f)             { OutWhy = FString::Printf(TEXT("Arm collapsed=%.1f"), CameraBoom->TargetArmLength); return true; }

	static float PrevArm = -1.f;
	static USceneComponent* PrevParent = nullptr;

	const bool bParentChanged  = PrevParent && FollowCamera->GetAttachParent() != PrevParent;
	const bool bArmSuddenDrop  = PrevArm >= 0.f && (PrevArm - CameraBoom->TargetArmLength) >= 100.f;
	const bool bFirstPersonish = CameraBoom->TargetArmLength <= 5.f;

	PrevArm = CameraBoom->TargetArmLength;
	PrevParent = FollowCamera->GetAttachParent();

	if (bParentChanged)      { OutWhy = TEXT("Camera parent CHANGED this tick"); return true; }
	if (bArmSuddenDrop)      { OutWhy = FString::Printf(TEXT("Arm sudden drop to %.1f"), CameraBoom->TargetArmLength); return true; }
	if (bFirstPersonish)     { OutWhy = FString::Printf(TEXT("Arm ~0 (FP) %.1f"), CameraBoom->TargetArmLength); return true; }
	
    return false;
}

void AFillainCharacter::CamWatchdogTick()
{
	if (!IsValid(this) || IsPendingKillPending()) return;
	if (!IsValid(GetWorld()) || !IsLocallyControlled()) return;

	const UCapsuleComponent* Cap = GetCapsuleComponent();
	if (!IsValid(Cap) || !IsValid(CameraBoom) || !IsValid(FollowCamera)) return;
	if (bCamFixCooldown) return;

	const float CapZ   = Cap->GetComponentLocation().Z;
	const float CamZ   = FollowCamera->GetComponentLocation().Z;
	const float DeltaZ = CamZ - CapZ;                 // <— define it here

	if (DeltaZ < -50.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ CamWatchdog: Camera below capsule ΔZ=%.1f — resetting"), DeltaZ);
		Client_NukeScreenOverlays();
		ResetCameraRig();
		bEquipInProgress = false;

		// throttle to avoid immediate re-trigger while transforms settle
		bCamFixCooldown = true;
		GetWorldTimerManager().SetTimer(CamFixCooldownHandle, this, &AFillainCharacter::CamWatchdogCooldownOff, 0.25f, false);
	}
}

void AFillainCharacter::StartCamWatchdog(float DurationSec, float TickSec)
{
    GetWorldTimerManager().SetTimer(CamWatchdogTimer, this, &AFillainCharacter::CamWatchdogTick, TickSec, true);

    FTimerHandle StopHandle;
    GetWorldTimerManager().SetTimer(StopHandle, [this]()
    {
        GetWorldTimerManager().ClearTimer(CamWatchdogTimer);
    }, DurationSec, false);
}

void AFillainCharacter::FixCameraIfWeird(const TCHAR* Tag)
{
	// no delay, no logging; just fix it now
	ResetCameraRig();
}

void AFillainCharacter::RestoreThirdPersonCameraSafe()
{
	if (!CameraBoom || !FollowCamera) return;

	// Temporarily disable collision so the boom can't push us into the floor mid-attach
	const bool bPrevCollision = CameraBoom->bDoCollisionTest;
	CameraBoom->bDoCollisionTest = false;

	// Make sure the camera is parented to the boom (not the weapon or mesh)
	FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);

	// Restore a sane default
	CameraBoom->TargetArmLength = FMath::Max(CameraBoom->TargetArmLength, 250.f);
	CameraBoom->SetRelativeLocation(FVector::ZeroVector);
	CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
	CameraBoom->bUsePawnControlRotation = true;

	// Ensure pawn uses controller yaw (typical 3P setup)
	if (APawn* P = Cast<APawn>(this)) { P->bUseControllerRotationYaw = false; }

	// Re-enable boom collision next tick (prevents immediate shove underground)
	GetWorldTimerManager().SetTimerForNextTick([this, bPrevCollision]()
	{
		if (CameraBoom) { CameraBoom->bDoCollisionTest = bPrevCollision; }
	});
}

void AFillainCharacter::RequestFillainCharacterCapsuleUpdate()
{
    // Schedule once for next tick so Agi/Flex updates caused by Dex settle first
    if (!GetWorld()) return;
    if (!GetWorldTimerManager().IsTimerActive(FillainCharacterCapsuleUpdateTimer))
    {
        GetWorldTimerManager().SetTimer(FillainCharacterCapsuleUpdateTimer, this,
            &AFillainCharacter::ApplyFillainCharacterCapsuleFromCurrentStats, 0.0f, false);
    }

	UE_LOG(LogTemp, Warning, TEXT("[Char] BeginPlay C (binding attrs)"));
	BindHiddenTreasureCapsuleHooksOnce();
	UE_LOG(LogTemp, Warning, TEXT("[Char] BeginPlay D (done)"));
}

void AFillainCharacter::ApplyFillainCharacterCapsuleFromCurrentStats()
{
    if (!ASC) return;

    // Read *current* attributes (Agi/Flex already reflect any Dex change)
    const float Agility     = ASC->GetNumericAttribute(UHAFAttributeSet::GetAgilityAttribute());
    const float Flexibility = ASC->GetNumericAttribute(UHAFAttributeSet::GetFlexibilityAttribute());

    const float AgiPct  = FMath::Clamp(Agility,     0.f, 100.f) * 0.01f;
    const float FlexPct = FMath::Clamp(Flexibility, 0.f, 100.f) * 0.01f;

    // Same formula you used in the MMC, but applied safely here:
    float TargetUnscaledHalf   = StandingUnscaledHalfHeight * (1.f - AgiPct);
    float TargetUnscaledRadius = StandingUnscaledRadius     * (1.f - FlexPct);

    // Clamp to sane mins so we never go too small
    TargetUnscaledHalf   = FMath::Clamp(TargetUnscaledHalf,   StandingUnscaledHalfHeight * 0.50f, StandingUnscaledHalfHeight);
    TargetUnscaledRadius = FMath::Clamp(TargetUnscaledRadius, StandingUnscaledRadius     * 0.60f, StandingUnscaledRadius);

    // Optional tiny change filter
    if (FMath::IsNearlyEqual(TargetUnscaledHalf,   LastAppliedHalf,   0.1f) &&
        FMath::IsNearlyEqual(TargetUnscaledRadius, LastAppliedRadius, 0.1f))
    {
        return;
    }

    ApplyFillainCharacterCapsuleSize_FeetPlanted(TargetUnscaledHalf, TargetUnscaledRadius);
    LastAppliedHalf   = TargetUnscaledHalf;
    LastAppliedRadius = TargetUnscaledRadius;
}

void AFillainCharacter::RestoreStandingFillainCharacterCapsule()
{
	ApplyFillainCharacterCapsuleSize_FeetPlanted(StandingUnscaledHalfHeight, StandingUnscaledRadius);
}

void AFillainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* DrawDebugSphere(
	GetWorld(),
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation(),
	25.0f,
	12,
	FColor::Magenta,
	false,
	0.f
); */

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
	RotateInPlace(DeltaTime);
	HideCharacterIfCameraClose();
	PollInit();

	if (!GetFillainPlayerController()) return;
	if (IsUsingGamepad()) Combat->TraceForCrossHairTarget(); else GetFillainPlayerController()->CursorTrace();
	
	if (IsLocallyControlled()) HideCharacterIfCameraClose();

	if (IsLocallyControlled() && bFOVLock && FollowCamera)
	{
		FOVLockTimeLeft -= DeltaTime;

		const float FOV = FollowCamera->FieldOfView;
		if (FOV < MinFOV || FOV > MaxFOV)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FOVGuard] Corrected FOV %.1f -> %.1f"), FOV, DefaultFOV);
			FollowCamera->SetFieldOfView(DefaultFOV);
		}

		if (FOVLockTimeLeft <= 0.f) bFOVLock = false;
	}
}

void AFillainCharacter::NotifyHit(
	UPrimitiveComponent* MyComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	UE_LOG(LogTemp, Warning, TEXT("🚧 BLOCKED by: %s (%s)"), *Other->GetName(), *OtherComp->GetName());
}

void AFillainCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("🎯 DirectionalHitReact triggered"));
	Super::DirectionalHitReact(ImpactPoint);
}

float AFillainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AFillainCharacter::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::HandleDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AFillainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFillainCharacter, OverlappingItem, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFillainCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AFillainCharacter, bDisableGameplay);
	DOREPLIFETIME(AFillainCharacter, EliminatedMontage);
	DOREPLIFETIME(AFillainCharacter, ReloadingMontage);
	DOREPLIFETIME(AFillainCharacter, ThrowGrenadeMontage);
	DOREPLIFETIME(AFillainCharacter, ArmDisarmMontage);
	
}


void AFillainCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void AFillainCharacter::HideSniperScope()
{
	ARangedWeapon* Gun = Cast<ARangedWeapon>(Combat->EquippedRangedWeapon);
	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedRangedWeapon && Gun && Gun->GetRangedType() == ERangedType::ERT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void AFillainCharacter::ShowSniperScope()
{
	ARangedWeapon* Gun = Cast<ARangedWeapon>(Combat->EquippedRangedWeapon);
	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedRangedWeapon && Gun && Gun->GetRangedType() == ERangedType::ERT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(true);
	}
}

void AFillainCharacter::Eliminate(bool bPlayerLeftGame)
{
	DropOrDestroyBothWeapons();
	MulticastEliminate(bPlayerLeftGame);
	GetWorldTimerManager().SetTimer(
		EliminationTimer,
		this,
		&AFillainCharacter::EliminationTimerFinished,
		EliminationDelay
	);
	
}

void AFillainCharacter::StartDissolveEffect()
{
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 300.f);
	}
	StartDissolve();
}

void AFillainCharacter::DisableAllComponents()
{
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	DisableCapsule();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFillainCharacter::SpawnEliminationBotEffect()
{
	if (EliminationBotEffect)
	{
		FVector EliminationBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		EliminationBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			EliminationBotEffect,
			EliminationBotSpawnPoint,
			GetActorRotation()
		);
	}
}

void AFillainCharacter::PlayEliminationSound()
{
	if (EliminationBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			EliminationBotSound,
			GetActorLocation()
		);
	}
}

void AFillainCharacter::DestroyCrown()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void AFillainCharacter::MulticastEliminate_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	if (VictimController)
	{
		VictimController->SetHUDWeaponAmmo(0);
	}
	bIsEliminated = true;
	UAnimInstance* AnimBlueprint = Cast<UAnimInstance>(GetMesh()->GetAnimInstance()); 
	PlayEliminatedMontage();
	StartDissolveEffect();
	DisableAllComponents();
	SpawnEliminationBotEffect();
	PlayEliminationSound();
	HideSniperScope();
	DestroyCrown();
	GetWorldTimerManager().SetTimer(
		EliminationTimer,
		this,
		&AFillainCharacter::EliminationTimerFinished,
		EliminationDelay
	);
}

void AFillainCharacter::EliminationTimerFinished()
{
	HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;
	if (HAFGameMode && !bLeftGame)
	{
		HAFGameMode->RequestRespawn(this, Controller);

	}
	if (bLeftGame && IsLocallyControlled())
	{
		PlayerLeavesGame.Broadcast();
	}
}

void AFillainCharacter::Heal(float Magnitude)
{
	if (!HealingEffect || !this) return;

	UAbilitySystemComponent* AbSysComp = GetAbilitySystemComponent();
	if (!AbSysComp) return;

	FGameplayEffectContextHandle Context = AbSysComp->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbSysComp->MakeOutgoingSpec(HealingEffect, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Healing")), Magnitude);
		AbSysComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AFillainCharacter::Fortify(float Magnitude)
{
	if (!ShieldFortifyingEffect || !this) return;

	UAbilitySystemComponent* AbSyCo = GetAbilitySystemComponent();
	if (!AbSyCo) return;

	FGameplayEffectContextHandle Context = AbSyCo->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbSyCo->MakeOutgoingSpec(ShieldFortifyingEffect, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Shield")), Magnitude);
		AbSyCo->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AFillainCharacter::Recharge(float Magnitude)
{
	if (!StaminaRechargingEffect || !this) return;

	UAbilitySystemComponent* ASComp = GetAbilitySystemComponent();
	if (!ASComp) return;

	FGameplayEffectContextHandle Context = ASComp->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASComp->MakeOutgoingSpec(StaminaRechargingEffect, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Stamina")), Magnitude);
		ASComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AFillainCharacter::Summon(float Magnitude)
{
	if (!MajixSummoningEffect || !this) return;

	UAbilitySystemComponent* ASysC = GetAbilitySystemComponent();
	if (!ASysC) return;

	FGameplayEffectContextHandle Context = ASysC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASysC->MakeOutgoingSpec(MajixSummoningEffect, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Majix")), Magnitude);
		ASysC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AFillainCharacter::ServerLeaveGame_Implementation()
{
	AHAFGameMode* GameMode = GetWorld()->GetAuthGameMode<AHAFGameMode>();
	HAFPlayerState = HAFPlayerState == nullptr ? GetPlayerState<AHAFPlayerState>() : HAFPlayerState;
	if (GameMode && HAFPlayerState)
	{
		GameMode->PlayerLeftGame(HAFPlayerState);

	}
}

void AFillainCharacter::DropOrDestroyWeapon(AWeaponBase* WeaponBase)
{
	if (WeaponBase == nullptr) return;
	if (WeaponBase->bDestroyWeapon)
	{
		WeaponBase->Destroy();
	}
	else
	{
		WeaponBase->WeaponDropped();
	}
}

void AFillainCharacter::DropOrDestroyBothWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void AFillainCharacter::OnPlayerStateInitialized()
{
	HAFPlayerState->AddToScore(0.f);
	HAFPlayerState->AddToDefeats(0);
	SetTeamColor(HAFPlayerState->GetTeam());
	SetSpawnPoint();
}

bool AFillainCharacter::CanDisarm()
{
	return Combat &&
		   EquippedWeapon &&
		   IfPlayerIsReadyToFightAgain() &&
		   IfPlayerHasEquippedAWeapon();
}

bool AFillainCharacter::IfPlayerIsDisarmed()
{
	return BattlePrepped == EBattlePrepped::EBP_Disarmed;
}

bool AFillainCharacter::CanArm()
{
	// UE_LOG(LogTemp, Warning, TEXT("CanArm? Equipped: %d, ActionState: %s, BattlePrepped: %s"),
		// Combat->EquippedWeapon != nullptr,
		// *UEnum::GetValueAsString(Combat->ActionState),
		// *UEnum::GetValueAsString(BattlePrepped));
	return Combat &&
		   EquippedWeapon &&
		   IfPlayerIsReadyToFightAgain() &&
		   IfPlayerIsDisarmed();
}

void AFillainCharacter::AttachWeaponToSpineSocket()
{
	if (Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AFillainCharacter::Disarm()
{
	if (Combat->EquippedWeapon->WeaponState == EWeaponState::EWS_EquippedOneHanded)
	{
		PlayArmDisarmMontage(FName("DisarmOneHanded"));
	}
	else if (Combat->EquippedWeapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded)
	{
		PlayArmDisarmMontage(FName("DisarmTwoHanded"));
	}
	AttachWeaponToSpineSocket();
	Combat->FightingStyle = EFightingStyle::EFS_Unequipped;
	Combat->ActionState = EActionState::EAS_EquippingWeapon;
	BattlePrepped = EBattlePrepped::EBP_Disarmed;
}

void AFillainCharacter::AttachWeaponToMeleeSocket()
{
	if (Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("MeleeSocket"));
		ResetToFightAgain();
	}
}

void AFillainCharacter::Arm()
{
	if (Combat->EquippedWeapon->WeaponState == EWeaponState::EWS_EquippedOneHanded)
	{
		PlayArmDisarmMontage(FName("ArmOneHanded"));
	}
	else if (Combat->EquippedWeapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded)
	{
		PlayArmDisarmMontage(FName("ArmTwoHanded"));
	}
	AttachWeaponToMeleeSocket();
	Combat->FightingStyle = EFightingStyle::EFS_Melee;
	Combat->ActionState = EActionState::EAS_EquippingWeapon;
	BattlePrepped = EBattlePrepped::EBP_Armed;
	ResetToFightAgain();
}

void AFillainCharacter::PlayArmDisarmMontage(const FName& SectionName)
{
	if (!ArmDisarmMontage) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(ArmDisarmMontage) || bIsTogglingWeapon)
	{
		return;
	}
	
	// Set state BEFORE playing
	Combat->ActionState = EActionState::EAS_EquippingWeapon;
	bIsTogglingWeapon = true;
	
	// Play and jump to section once
	AnimInstance->Montage_Play(ArmDisarmMontage);
	AnimInstance->Montage_JumpToSection(SectionName, ArmDisarmMontage);
}

void AFillainCharacter::OnArmDisarmMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ArmDisarmMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, ArmDisarmMontage);  // Just to be safe
		UE_LOG(LogTemp, Warning, TEXT("✅ Arm/Disarm Montage Ended"));
		bIsTogglingWeapon = false;
		ResetToFightAgain();  // If this resets animation state or booleans
	}
}

void AFillainCharacter::FinishEquipping()
{
	ResetToFightAgain();
}


void AFillainCharacter::SetSpawnPoint()
{
	if (HasAuthority() && HAFPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == HAFPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		if (TeamPlayerStarts.Num() > 0)
		{
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			SetActorLocationAndRotation(
				ChosenPlayerStart->GetActorLocation(), 
				ChosenPlayerStart->GetActorRotation()
			);
		}
	}
}

void AFillainCharacter::Destroyed()
{
	Super::Destroyed();
	if (EliminationBotComponent)
	{
		EliminationBotComponent->DestroyComponent();
	}

	HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;
	bool bIsMatchNotInProgress = HAFGameMode && HAFGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bIsMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void AFillainCharacter::HideAttachedGrenade()
{
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}


void AFillainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (!IsValid(this)) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	Combat->ActionState = EActionState::EAS_HitReaction;
}

void AFillainCharacter::PlayHitReactMontage(const FName& SectionName)
{
	Super::PlayHitReactMontage(SectionName);
}

void AFillainCharacter::RotateInPlace(float DeltaTime)
{
	if (Combat && Combat->bWieldingTheSword)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (Combat && Combat->EquippedWeapon) GetCharacterMovement()->bOrientRotationToMovement = false;
	if (Combat && Combat->EquippedWeapon) bUseControllerRotationYaw = true;
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void AFillainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UHAFInputComponent* HAFInputComponent = CastChecked<UHAFInputComponent>(PlayerInputComponent))

	{
		HAFInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Move);
		HAFInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Look);
		HAFInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFillainCharacter::Jump);
		HAFInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AFillainCharacter::EquipButtonPressed);
		HAFInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AFillainCharacter::CrouchButtonPressed);
		HAFInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AFillainCharacter::AimButtonPressed);
		HAFInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AFillainCharacter::AimButtonReleased);
		HAFInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AFillainCharacter::AttackButtonPressed);
		HAFInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AFillainCharacter::AttackButtonReleased);
		HAFInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AFillainCharacter::ReloadButtonPressed);
		HAFInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AFillainCharacter::GrenadeButtonPressed);
		HAFInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Dodge);
	}
}

void AFillainCharacter::AddSoulsGatheredToTotalSouls(class ASoul* Soul)
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Souls: %d"), Soul->GetSoulValue());

	if (AttributeComponent)
	{
		AttributeComponent->UpdateTotalSouls(AttributeComponent->GetSoulsGathered());

		AFillainPlayerController* PlayerController = Cast<AFillainPlayerController>(Controller);
		PlayerController->SetHUDSoulsCount(AttributeComponent->GetSoulsGathered());
	}
}

void AFillainCharacter::AddGoldAcquiredToTotalGold(class ATreasure* Treasure)
{
	if (AttributeComponent)
	{
		AttributeComponent->UpdateTotalGold(AttributeComponent->GetGoldAcquired());

		AFillainPlayerController* PlayerController = Cast<AFillainPlayerController>(Controller);
		PlayerController->SetHUDGoldCount(AttributeComponent->GetGoldAcquired());
	}
}

int32 AFillainCharacter::GetPlayerLevel()
{
	const AHAFPlayerState* State = GetPlayerState<AHAFPlayerState>();
	check(State);
	return State->GetPlayerLevel();
	
}

double AFillainCharacter::GetCharacterCapsuleHeight()
{
	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

double AFillainCharacter::GetCharacterCapsuleRadius()
{
	return GetCapsuleComponent()->GetScaledCapsuleRadius();
}

void AFillainCharacter::SetCharacterCapsuleHeight(double Height)
{
	float Percent = Height / 100.f;
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCharacterCapsuleHeight() - (Percent * GetCharacterCapsuleHeight()));
}

void AFillainCharacter::SetCharacterCapsuleRadius(double Radius)
{
	float Percent = Radius / 100.f;
	GetCapsuleComponent()->SetCapsuleRadius(GetCharacterCapsuleRadius() - (Percent * GetCharacterCapsuleRadius()));
}

void AFillainCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AFillainCharacter::ActivateCombatCharacter()
{
	if (Combat)
	{
		Combat->Character = this;
	}
}

void AFillainCharacter::InitializeBuffProperties()
{
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeed(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}

void AFillainCharacter::ConfigureLagCompensation()
{
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AFillainPlayerController>(Controller);
		}
	}
}

void AFillainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ActivateCombatCharacter();
	InitializeBuffProperties();
	ConfigureLagCompensation();
}

bool AFillainCharacter::IsWeaponASword()
{
	return Combat->EquippedWeapon->WeaponCategory == EWeaponCategory::EWC_Sword ||
		Combat->EquippedWeapon->WeaponCategory == EWeaponCategory::EWC_OneHandedSword ||
		Combat->EquippedWeapon->WeaponCategory == EWeaponCategory::EWC_TwoHandedSword;
}

bool AFillainCharacter::PlayMeleeMontageForMeleeWeapons()
{
	if (IsWeaponASword())
	{
		PlayMeleeAttackMontage();
		return true;
	}
	return false;
}

void AFillainCharacter::PlayRangedAnimationsForRangedWeapons(bool bAiming)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAiming") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

bool AFillainCharacter::IsPlayerWeaponlessAndUnableToCombat()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return true;
	return false;
}

void AFillainCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAiming") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AFillainCharacter::AssignTypeOfRangedWeapon(AWeaponBase* Weapon, FName SectionName)
{
	ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(Weapon);
	if (Combat->EquippedRangedWeapon)
	{
		RangedWeapon = Combat->EquippedRangedWeapon;
		switch (Combat->EquippedRangedWeapon->GetRangedType())
		{
		case ERangedType::ERT_AssaultRifle:
			SectionName = FName("AssaultRifle");
			break;
		case ERangedType::ERT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case ERangedType::ERT_Pistol:
		case ERangedType::ERT_SubmachineGun: // share section
			SectionName = FName("Pistol");
			break;
		case ERangedType::ERT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case ERangedType::ERT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case ERangedType::ERT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		default:
			break;
		}
	}
}
	
void AFillainCharacter::PlayReloadingMontage()
{
	if (IsPlayerWeaponlessAndUnableToCombat()) return;

	FString NameAsString = GetRangedWeaponName();
	FName NameAsFName = FName(*NameAsString);
	AssignTypeOfRangedWeapon(EquippedWeapon, NameAsFName);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadingMontage)
	{
		AnimInstance->Montage_Play(ReloadingMontage);
		
		// Default value in case none matches
		FName SectionName = NAME_None;

		if (SectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(SectionName, ReloadingMontage);
		}
		else if (Combat->EquippedMeleeWeapon) return;
	}
}

void AFillainCharacter::PlayEliminatedMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EliminatedMontage)
	{
		AnimInstance->Montage_Play(EliminatedMontage);
	}
}

void AFillainCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void AFillainCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void AFillainCharacter::AttackButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("🎯 AttackButtonPressed — EquippedWeapon: %s"),
	Combat->EquippedWeapon ? *Combat->EquippedWeapon->GetName() : TEXT("nullptr"));
	ResetToFightAgain();
	if (Combat->EquippedWeapon)
	{
		if (EquippedWeaponIsARangedWeapon())
		{
			FireButtonPressed();
		}
		else if (EquippedWeaponIsAMeleeWeapon())
		{
			Combat->ActionState = EActionState::EAS_Unoccupied;
			Combat->FightingStyle = EFightingStyle::EFS_Melee;
			MeleeAttack();
		}
	}
}



void AFillainCharacter::MeleeAttack()
{
	if (CanAttack())
	{
		Combat->ActionState = EActionState::EAS_MeleeAttacking;
		PlayMeleeAttackMontage();
		
		// Don’t set ActionState back to Unoccupied here anymore. Let the montage handle it via Notify.
	}
}

bool AFillainCharacter::IsOccupied()
{
	return Combat->ActionState != EActionState::EAS_Unoccupied;
}

bool AFillainCharacter::HasEnoughStamina(const float Cost) const
{
	if (!AbilitySystemComponent) return false;

	const float Stamina =
		AbilitySystemComponent->GetNumericAttribute(UHAFAttributeSet::GetStaminaAttribute());
	return Stamina >= Cost;}

void AFillainCharacter::Dodge()
{
	if (!AbilitySystemComponent) return;

	if (!bASCReady || !AbilitySystemComponent) return;
	if (!HasEnoughStamina(AttributeComponent->GetDodgeCost())) return;

	// Either use cached pointer (after InitASC ran on this machine)…
	const UHAFAttributeSet* AS = HAFAttributeSet ? HAFAttributeSet
												 : AbilitySystemComponent->GetSet<UHAFAttributeSet>();
	if (!AS) { UE_LOG(LogTemp, Warning, TEXT("AttributeSet null in Dodge")); return; }

	const float Stamina = AS->GetStamina();  // now safe
	if (IsOccupied() || !HasEnoughStamina(AttributeComponent->GetDodgeCost())) return;

	PlayDodgeMontage();
	Combat->ActionState = EActionState::EAS_Dodging;

	if (AttributeComponent)
	{
		AttributeComponent->UseStamina(AttributeComponent->GetDodgeCost());
	}
}


void AFillainCharacter::FireButtonPressed()
{
	if (EquippedWeaponIsAMeleeWeapon()) return;
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AFillainCharacter::FireButtonReleased()
{
	if (EquippedWeaponIsAMeleeWeapon()) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

int32 AFillainCharacter::PlayMeleeAttackMontage()
{
	Super::PlayMeleeAttackMontage();
	return PlayRandomMontageSection(MeleeAttackMontage, MeleeAttackMontageSections);
}

void AFillainCharacter::ResetToFightAgain()
{
	Combat->ActionState = EActionState::EAS_Unoccupied;
}

void AFillainCharacter::AttackEnd()
{
	Super::AttackEnd();
	if (Combat)
	{
		Combat->ActionState = EActionState::EAS_Unoccupied;
	}
}

void AFillainCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	Combat->ActionState = EActionState::EAS_Unoccupied;
}

bool AFillainCharacter::IfPlayerIsReadyToFightAgain()
{
	return Combat->ActionState == EActionState::EAS_Unoccupied;
}

bool AFillainCharacter::IfPlayerHasEquippedAWeapon()
{
	return Combat->FightingStyle != EFightingStyle::EFS_Unequipped;
}

bool AFillainCharacter::CanAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("CanAttack: ActionState = %d | FightingStyle = %d"), (int32)Combat->ActionState, (int32)Combat->FightingStyle);
	UE_LOG(LogTemp, Warning, TEXT("Checking CanAttack for: %s"), *GetName());

	return IfPlayerIsReadyToFightAgain() && IfPlayerHasEquippedAWeapon();

}

void AFillainCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void AFillainCharacter::DetermineRolesOnPlayerDeath(AActor* DamagedPawn, AController* InstigatorController)
{
	HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;

	if (HAFGameMode)
	{
		FillainPlayerController = FillainPlayerController == nullptr ? Cast<AFillainPlayerController>(Controller) : FillainPlayerController;
		AFillainPlayerController* KillerController = Cast<AFillainPlayerController>(InstigatorController);
		AFillainCharacter* KillerFillain = Cast<AFillainCharacter>(InstigatorController->GetPawn());
		AFillainCharacter* VictimFillain = Cast<AFillainCharacter>(DamagedPawn);
		AFillainPlayerController* ControllerOfVictim = Cast<AFillainPlayerController>(VictimFillain->GetController());
		HAFGameMode->PlayerEliminated(this, ControllerOfVictim, KillerController);
		ControllerOfVictim->InitializeHUDEliminationMessage(KillerController, ControllerOfVictim, CachedEventInstigator);
		KillerController->InitializeHUDEliminationMessage(KillerController, ControllerOfVictim, CachedEventInstigator);
	}
}

void AFillainCharacter::ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedPawn, Damage, DamageType, InstigatorController, DamageCauser);

	HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;
	if (bIsEliminated || HAFGameMode == nullptr) return;

	// Use the AttributeComponent safely
	UAttributeComponent* AC = AttributeComponent;
	if (!IsValid(AC))
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeComponent is null on %s"), *GetName());
		return;
	}

	// Correct shield/health damage application:
	// 1) Apply to shield first
	const float ShieldBefore = AC->GetShield();
	const float DamageToShield = FMath::Min(ShieldBefore, Damage);
	const float OverflowDamage = Damage - DamageToShield;

	AC->SetShield(FMath::Clamp(ShieldBefore - Damage, 0.f, AC->GetMaxShield()));

	// 2) Apply overflow to health
	AC->SetHealth(FMath::Clamp(AC->GetHealth() - OverflowDamage, 0.f, AC->GetMaxHealth()));

	// If you need HUD access, guard it:
	// APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	// AFillainHUD* FillainHUD = PC ? Cast<AFillainHUD>(PC->GetHUD()) : nullptr;
	// if (FillainHUD && FillainHUD->OverlayWidget) { /* update overlay as needed */ }

	// Handle elimination purely based on health (don't gate on HUD availability)
	if (AC->GetHealth() <= 0.f)
	{
		if (HAFGameMode)
		{
			FillainPlayerController = FillainPlayerController == nullptr ? Cast<AFillainPlayerController>(Controller) : FillainPlayerController;
			KillerPlayerController = Cast<AFillainPlayerController>(InstigatorController);
			DetermineRolesOnPlayerDeath(DamagedPawn, InstigatorController);
			HAFGameMode->PlayerEliminated(this, FillainPlayerController, KillerPlayerController);
		}
	}
}

void AFillainCharacter::SpawnDefaultWeapon()
{
	HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;
	UWorld* World = GetWorld();
	if (HAFGameMode && World && !bIsEliminated && DefaultWeaponClass)
	{
		AWeaponBase* StartingWeapon = World->SpawnActor<AWeaponBase>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AFillainCharacter::SpawnCrown()
{
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CrownSystem, GetMesh(), FName(), GetActorLocation() + FVector(0.f, 0.f, 110.f), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
}

void AFillainCharacter::ActivateCrown()
{
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void AFillainCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	SpawnCrown();
	ActivateCrown();
}

void AFillainCharacter::MulticastLostTheLead_Implementation()
{
	DestroyCrown();
}

void AFillainCharacter::SetTeamColor(ETeam Team)
{
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = OriginalDissolveMaterialInstance;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMaterialInstance;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMaterialInstance;
		break;
	}
}

bool AFillainCharacter::IsUsingGamepad() const
{
	if (ULocalPlayer* LocalPlayer = GetFillainPlayerController()->GetLocalPlayer())
	{
		if (UCommonInputSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UCommonInputSubsystem>())
		{
			ECommonInputType InputType = InputSubsystem->GetCurrentInputType();
			return InputType == ECommonInputType::Gamepad;
		}
	}
	return false;
}

void AFillainCharacter::Move(const FInputActionValue& Value)
{
	if (Combat->ActionState != EActionState::EAS_Unoccupied) return;
	if (bDisableGameplay)
	{
		bDisableGameplay = false;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Deadzone filtering
	if (MovementVector.IsNearlyZero(0.3f)) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (!MovementVector.IsNearlyZero(0.01f))
	{
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	
	//UE_LOG(LogTemp, Warning, TEXT("MovementVector: %s"), *MovementVector.ToString());
}


void AFillainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}

	//UE_LOG(LogTemp, Warning, TEXT("LookAxisVector: %s"), *LookAxisVector.ToString());

}


void AFillainCharacter::EquipButtonPressed()
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	

	if (CharactersMeleeWeapon)
	{
		ToggleArmingAndDisarming();
		return;
	}
	
	if (bDisableGameplay || (Combat && Combat->ActionState != EActionState::EAS_Unoccupied))
	{
		UE_LOG(LogTemp, Warning, TEXT("🚫 Equip failed: gameplay disabled or action state is not unoccupied"));
		return;
	}

	if (ARangedWeapon* RangedWeaponToEquip = Cast<ARangedWeapon>(OverlappingWeapon))
	{
		ServerEquipButtonPressed(RangedWeaponToEquip);
		SetOverlappingItem(nullptr);
		SetOverlappingWeapon(nullptr);
		if (!IsValid(RangedWeaponToEquip))
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ No valid weapon to equip"));
			return;
		}

		// ✅ Send to server
		ServerEquipButtonPressed(OverlappingWeapon);

		// ✅ Clean up local overlap (Dark Souls style)
		SetOverlappingItem(nullptr);
		SetOverlappingWeapon(nullptr);

		if (bEquipInProgress) return;

		// Snapshot the weapon NOW
		AWeaponBase* LocalWeapon = OverlappingWeapon;
		UE_LOG(LogEquipTrace, Warning, TEXT("[%s] EquipButtonPressed | Snapshot Weapon=%s"),
			*GetName(), LocalWeapon ? *LocalWeapon->GetName() : TEXT("None"));

		if (!LocalWeapon) return;

		bEquipInProgress = true; // debounce immediately
		ServerEquipWeapon(LocalWeapon); // pass the pointer, don't re-read OverlappingWeapon later
		if (FollowCamera)
		{
			FollowCamera->SetFieldOfView(DefaultFOV);
			bFOVLock = true;
			FOVLockTimeLeft = 0.75f; // hold for ~¾s; tweak if needed
		}
	}
	else return;
}

void AFillainCharacter::ServerEquipWeapon_Implementation(AWeaponBase* WeaponToEquip)
{

}

void AFillainCharacter::ToggleArmingAndDisarming()
{
	// Equip visually
	if (CanDisarm())
	{
		CharactersWeapon->SetHandsNeeded(CharactersWeapon);
		if (CharactersWeapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
			DisarmOneHandedWeapon(CharactersMeleeWeapon);
		else if (CharactersWeapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
			DisarmTwoHandedWeapon(CharactersMeleeWeapon);
	}
	else if (CanArm())
	{
		CharactersWeapon->SetHandsNeeded(CharactersWeapon);
		if (CharactersWeapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
			ArmOneHandedWeapon(CharactersMeleeWeapon);
		else if (CharactersWeapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
			ArmTwoHandedWeapon(CharactersMeleeWeapon);
	}
}

void AFillainCharacter::ServerEquipButtonPressed_Implementation(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	
	if (!Combat || !Weapon) return;

	if (Combat->ActionState == EActionState::EAS_EquippingWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Already equipping — ignoring input"));
		return;
	}

	// Disarm/arm toggle
	if (EquippedWeapon && EquippedWeapon->ItemState == EItemState::EIS_Equipped)
	{
		ToggleArmingAndDisarming();
		return;
	}
	UE_LOG(LogEquipTrace, Warning, TEXT("[%s] ServerEquipWeapon | WeaponToEquip=%s"),
			*GetName(), OverlappingWeapon ? *OverlappingWeapon->GetName() : TEXT("None"));

	if (!OverlappingWeapon || !IsValid(OverlappingWeapon)) { bEquipInProgress = false; return; }

	// Do the equip work (attach, set owner, set Combat refs, play montage, etc.)
	Combat->EquipWeapon(OverlappingWeapon);
	Weapon = OverlappingWeapon;
	Client_OnEquipped();
	FixSelfCameraCollision();

	

	// After the weapon is attached/owned:
	{
		// 1) Lights (HoverLight / flare lights)
		TArray<ULightComponent*> Lights;
		Weapon->GetComponents(Lights);
		for (ULightComponent* L : Lights)
		{
			if (!L) continue;
			L->SetVisibility(false, true);
			L->SetIntensity(0.f);
		}

		// 2) Niagara / Cascade FX (muzzle flash, glows)
		TArray<UNiagaraComponent*> NComps;
		Weapon->GetComponents(NComps);
		for (UNiagaraComponent* N : NComps)
		{
			if (!N) continue;
			N->Deactivate();
			N->SetAutoActivate(false);
		}

		// 3) Decals / 3D widgets used for pickup prompts
		TArray<UDecalComponent*> Decals;
		Weapon->GetComponents(Decals);
		for (UDecalComponent* D : Decals)
		{
			if (!D) continue;
			D->SetVisibility(false, true);
		}

		// (Optional) If you have a specific HoverLight pointer, just disable/destroy it:
		// if (WeaponToEquip->HoverLight) WeaponToEquip->HoverLight->DestroyComponent();
	}
	
	// Now that we’re done, clear overlaps on the server
	OverlappingItem = nullptr;
	OverlappingWeapon = nullptr;

	// Option A: clear on montage end via delegate
	// Option B: clear now and reset the flag
	bEquipInProgress = false;

	UE_LOG(LogTemp, Warning, TEXT("✅ Cleared overlapping references after successful equip"));
	Client_PostEquipCameraFix();

	// Actual equip logic
	if (Weapon->IsA(ARangedWeapon::StaticClass()))
	{
		Combat->EquipWeapon(Weapon);
		return;
	}

	if (AMeleeWeapon* Melee = Cast<AMeleeWeapon>(Weapon))
	{
		Combat->EquipWeapon(Melee);
		EquippedWeapon = Melee;
		Combat->EquippedWeapon = Melee;
		CharactersWeapon = Melee;
		CharactersMeleeWeapon = Melee;

		// Set proper collision
		if (Melee->WeaponBox)
		{
			Melee->WeaponBox->SetCollisionObjectType(ECC_PCWeaponBox);
			Melee->WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Melee->WeaponBox->SetGenerateOverlapEvents(true);
			Melee->WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
			Melee->WeaponBox->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
		}
	}

	if (Weapon && Weapon->ItemState == EItemState::EIS_Equipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Server equipped %s"), *Weapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Weapon not marked equipped after Equip call"));
	}
	StartCamWatchdog(2.0f);
}


void AFillainCharacter::EquipWeapon(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	if (!Weapon) return;

	UE_LOG(LogTemp, Warning, TEXT("AFillainCharacter::EquipWeapon() called for: %s"), *Weapon->GetName());

	if (Weapon->IsA(ARangedWeapon::StaticClass()))
	{
		if (Weapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
		{
			EquipOneHandedRangedWeapon(Weapon);
		}
		else if (Weapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
		{
			EquipTwoHandedRangedWeapon(Weapon);
		}
	}
	else if (Weapon->IsA(AMeleeWeapon::StaticClass()))
	{
		if (Weapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
		{
			EquipOneHandedMeleeWeapon(Weapon);
		}
		else if (Weapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
		{
			EquipTwoHandedMeleeWeapon(Weapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Unknown weapon type or hands needed — cannot equip: %s"), *Weapon->GetName());
	}
	if (FollowCamera && CameraBoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("Before Equip -> Camera Location: %s | Boom Length: %f | AttachedTo: %s"),
			*FollowCamera->GetComponentLocation().ToString(),
			CameraBoom->TargetArmLength,
			*FollowCamera->GetAttachParent()->GetName());
	}

	// ⚡ Your existing equip logic here
	// (Attach weapon, set state, play montage, etc.)

	if (FollowCamera && CameraBoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("After Equip -> Camera Location: %s | Boom Length: %f | AttachedTo: %s"),
			*FollowCamera->GetComponentLocation().ToString(),
			CameraBoom->TargetArmLength,
			*FollowCamera->GetAttachParent()->GetName());
	}
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(DefaultFOV);
		bFOVLock = true;
		FOVLockTimeLeft = 0.75f; // hold for ~¾s; tweak if needed
	}
}

void AFillainCharacter::ResetCameraRig()
{
	// 1) Ensure hierarchy: Camera attached to SpringArm, SpringArm to Root
	if (CameraBoom->GetAttachParent() != GetRootComponent())
	{
		CameraBoom->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CameraBoom->SetRelativeLocation(FVector::ZeroVector);
		CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
		CameraBoom->SetRelativeScale3D(FVector::OneVector);
	}

	if (FollowCamera->GetAttachParent() != CameraBoom)
	{
		FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
										USpringArmComponent::SocketName);
		FollowCamera->SetRelativeLocation(FVector::ZeroVector);
		FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
	}

	// 2) Restore spring arm safety settings
	CameraBoom->TargetArmLength = FMath::Max(50.f, DefaultArmLength); // never negative/zero
	CameraBoom->SocketOffset    = FVector::ZeroVector;
	CameraBoom->TargetOffset    = DefaultTargetOffset;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;     // keep sweep to avoid world clipping
	CameraBoom->bEnableCameraLag = false;    // optional: disable to avoid late-frame dips

	// 3) Camera flags
	FollowCamera->bUsePawnControlRotation = false;

	// 4) Nuke any stray world transforms (if something set world loc directly)
	FollowCamera->SetWorldTransform(CameraBoom->GetSocketTransform(USpringArmComponent::SocketName));

	// Breadcrumb
	UE_LOG(LogTemp, Warning, TEXT("[ResetCameraRig] Parent=%s Arm=%.1f CamZ=%.1f CapZ=%.1f"),
		   *GetNameSafe(CameraBoom->GetAttachParent()), CameraBoom->TargetArmLength,
		   FollowCamera->GetComponentLocation().Z, GetCapsuleComponent()->GetComponentLocation().Z);
}

void AFillainCharacter::Client_SafeViewAfterEquip_Implementation()
{
	// 0) Kill any legacy timers on this actor (including that [PostEquip+600ms] one)
    GetWorldTimerManager().ClearAllTimersForObject(this);

    // 1) Make the boom/camera immune to self-collision & exposure flicker
    if (CameraBoom) CameraBoom->bDoCollisionTest = false;

    if (FollowCamera)
    {
        FollowCamera->PostProcessSettings = FPostProcessSettings(); // reset PP
        auto& PPS = FollowCamera->PostProcessSettings;
        PPS.bOverride_AutoExposureMinBrightness = true;
        PPS.bOverride_AutoExposureMaxBrightness = true;
        PPS.AutoExposureMinBrightness = 1.0f;
        PPS.AutoExposureMaxBrightness = 1.0f;
    }

    // 2) Hide my own mesh/weapon for the owning player only
    if (USkeletalMeshComponent* SMC = GetMesh())
        SMC->SetOwnerNoSee(true);

    if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
        Combat->EquippedWeapon->GetWeaponMesh()->SetOwnerNoSee(true);

    // 3) Mute weapon visuals that can full-screen you (lights/FX/widgets/decals)
    if (Combat && Combat->EquippedWeapon)
    {
        AActor* W = Combat->EquippedWeapon;

        TArray<ULightComponent*> Lights;       W->GetComponents(Lights);
        for (ULightComponent* L : Lights) if (L) { L->SetVisibility(false, true); L->SetIntensity(0.f); }

        TArray<UNiagaraComponent*> NFX;        W->GetComponents(NFX);
        for (UNiagaraComponent* N : NFX) if (N) { N->Deactivate(); N->SetAutoActivate(false); }

        TArray<UWidgetComponent*> Widgets;     W->GetComponents(Widgets);
        for (UWidgetComponent* WC : Widgets) if (WC) { WC->SetVisibility(false, true); WC->SetHiddenInGame(true, true); }

        TArray<UDecalComponent*> Decals;       W->GetComponents(Decals);
        for (UDecalComponent* D : Decals) if (D) { D->SetVisibility(false, true); }
    }

    // 4) Also hide any 3D widgets attached to the character
    {
        TArray<UWidgetComponent*> MyWidgets; GetComponents(MyWidgets);
        for (UWidgetComponent* WC : MyWidgets) if (WC) { WC->SetVisibility(false, true); WC->SetHiddenInGame(true, true); }
    }

    UE_LOG(LogTemp, Warning, TEXT("[SafeView] Applied: boom collision OFF, PP reset, owner meshes hidden, weapon lights/FX/widgets OFF."));
}

void AFillainCharacter::Client_ForceFollowCamera_Implementation()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// 1) Deactivate ALL camera components on self
	{
		TArray<UCameraComponent*> Cams;
		GetComponents<UCameraComponent>(Cams);
		for (UCameraComponent* C : Cams) if (C) C->Deactivate();
	}

	// 2) Deactivate ALL camera components on attached actors (weapon, etc.)
	{
		TArray<AActor*> Attached;
		GetAttachedActors(Attached, /*bIncludeFromChildActors=*/true, /*bIncludeSocketNames=*/true);
		for (AActor* A : Attached)
		{
			if (!A) continue;
			TArray<UCameraComponent*> Cams;
			A->GetComponents<UCameraComponent>(Cams);
			for (UCameraComponent* C : Cams) if (C) C->Deactivate();
		}
	}

	// 3) Activate the one true camera and set view target to THIS pawn
	if (FollowCamera) FollowCamera->Activate(true);
	PC->AutoManageActiveCameraTarget(this);
	PC->SetViewTarget(this); // no blend

	// 4) Prove what’s actually being used
	const FVector PCM = (PC->PlayerCameraManager) ? PC->PlayerCameraManager->GetCameraLocation() : FVector::ZeroVector;
	const FVector FCam = FollowCamera ? FollowCamera->GetComponentLocation() : FVector::ZeroVector;

	UE_LOG(LogTemp, Warning, TEXT("[CamForce] VT=%s  PCM=%s  FollowCam=%s  Match=%d"),
		*GetNameSafe(PC->PlayerCameraManager ? PC->PlayerCameraManager->GetViewTarget() : nullptr),
		*PCM.ToCompactString(), *FCam.ToCompactString(),
		(FVector::Dist(PCM, FCam) < 2.f) ? 1 : 0);
}

void AFillainCharacter::CamWatchdogCooldownOff()
{
	bCamFixCooldown = false;
	GetWorldTimerManager().ClearTimer(CamFixCooldownHandle);
	UE_LOG(LogTemp, Verbose, TEXT("[CamWatchdog] Cooldown off"));
}

void AFillainCharacter::Client_NukeScreenOverlays_Implementation()
{
	auto KillPPOnActor = [](AActor* A)
	{
		if (!A) return;

		// 1) Disable any PostProcessComponents (common on weapons/scopes/VFX)
		TArray<UPostProcessComponent*> PPCs;
		A->GetComponents<UPostProcessComponent>(PPCs);
		for (UPostProcessComponent* PPC : PPCs)
		{
			if (!PPC) continue;
			PPC->bEnabled = false;
			PPC->bUnbound = false;
			PPC->Settings.WeightedBlendables.Array.Reset();
			UE_LOG(LogTemp, Warning, TEXT("[NukePP] Disabled PostProcessComponent on %s"), *GetNameSafe(A));
		}

		// 2) Clear camera PostProcessSettings (in case the weapon wrote into your FollowCamera)
		TArray<UCameraComponent*> Cams;
		A->GetComponents<UCameraComponent>(Cams);
		for (UCameraComponent* Cam : Cams)
		{
			if (!Cam) continue;
			Cam->PostProcessSettings = FPostProcessSettings(); // reset to defaults
			UE_LOG(LogTemp, Warning, TEXT("[NukePP] Cleared PostProcessSettings on camera %s (%s)"),
				   *GetNameSafe(Cam), *GetNameSafe(A));
		}
	};

	// Self
	KillPPOnActor(this);

	// Everything attached (includes the newly equipped weapon)
	TArray<AActor*> Attached;
	GetAttachedActors(Attached, /*bIncludeFromChildActors=*/true, /*bIncludeSocketNames=*/true);
	for (AActor* A : Attached) { KillPPOnActor(A); }
}

void AFillainCharacter::Client_PostEquipCameraFix_Implementation()
{
	// If your attach/montage completes next tick, a tiny delay avoids racing transforms.
	GetWorldTimerManager().SetTimerForNextTick(this, &AFillainCharacter::ResetCameraRig);
}

void AFillainCharacter::EquipOneHandedRangedWeapon(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));

	if (Weapon->IsA(ARangedWeapon::StaticClass()) && Weapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
	{
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		Combat->FightingStyle = EFightingStyle::EFS_Ranged;
		Weapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
		Weapon->WeaponCategory = EWeaponCategory::EWC_OneHandedFirearm;
		EquippedWeapon = Weapon;
		Combat->EquippedWeapon = Weapon;
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(Weapon);
		CharactersRangedWeapon = RangedWeapon;
		Combat->EquippedRangedWeapon = RangedWeapon;
		CharactersRangedWeapon = RangedWeapon;
		Combat->EquipWeapon(RangedWeapon);
	}
	if (EquippedWeapon != nullptr && Combat->EquippedWeapon != nullptr)
	{
		OverlappingWeapon = nullptr;
		OverlappingItem = nullptr;
	}
}

void AFillainCharacter::EquipTwoHandedRangedWeapon(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	
	
	if (Weapon->IsA(ARangedWeapon::StaticClass()) && Weapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
	{
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		Combat->FightingStyle = EFightingStyle::EFS_Ranged;
		Weapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		Weapon->WeaponCategory = EWeaponCategory::EWC_TwoHandedFirearm;
		EquippedWeapon = Weapon;
		Combat->EquippedWeapon = Weapon;
		ARangedWeapon* RangedWeapon = Cast<ARangedWeapon>(Weapon);
		CharactersRangedWeapon = RangedWeapon;
		Combat->EquippedRangedWeapon = RangedWeapon;
		CharactersRangedWeapon = RangedWeapon;
		Combat->EquippedRangedWeapon = RangedWeapon;
		Combat->EquipWeapon(RangedWeapon);
	}
	if (EquippedWeapon != nullptr && Combat->EquippedWeapon != nullptr)
	{
		OverlappingWeapon = nullptr;
		OverlappingItem = nullptr;
	}
}

void AFillainCharacter::EquipOneHandedMeleeWeapon(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	
	
	if (Weapon->IsA(AMeleeWeapon::StaticClass()) && Weapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
	{
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		Combat->FightingStyle = EFightingStyle::EFS_Melee;
		Weapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
		Weapon->WeaponCategory = EWeaponCategory::EWC_OneHandedSword;
		EquippedWeapon = Weapon;
		Combat->EquippedWeapon = Weapon;
		AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(Weapon);
		CharactersMeleeWeapon = MeleeWeapon;
		Combat->EquippedMeleeWeapon = MeleeWeapon;
		DisarmOneHandedWeapon(MeleeWeapon);
		ArmOneHandedWeapon(MeleeWeapon);
		CharactersMeleeWeapon = MeleeWeapon;
		Combat->EquippedMeleeWeapon = MeleeWeapon;
	}
	if (EquippedWeapon != nullptr && Combat->EquippedWeapon != nullptr)
	{
		OverlappingWeapon = nullptr;
		OverlappingItem = nullptr;
	}
}

void AFillainCharacter::EquipTwoHandedMeleeWeapon(AWeaponBase* Weapon)
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	
	
	if (Weapon->IsA(AMeleeWeapon::StaticClass())&& Weapon->HandsNeeded == EHandsNeeded::EHN_TwoHandedWeapon)
	{
		if (Weapon->IsA(AMeleeWeapon::StaticClass()) && Weapon->HandsNeeded == EHandsNeeded::EHN_OneHandedWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("🧠 Inside IsA+HandsNeeded block, calling Weapon->Equip"));
			Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
			Combat->FightingStyle = EFightingStyle::EFS_Melee;
			Weapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
			Weapon->WeaponCategory = EWeaponCategory::EWC_TwoHandedSword;
			EquippedWeapon = Weapon;
			CharactersWeapon = Weapon;
			Combat->EquippedWeapon = Weapon;

			UE_LOG(LogTemp, Warning, TEXT("Weapon is a %s, IsA(Melee): %d"),
			   *Weapon->GetClass()->GetName(),
			   Weapon->IsA(AMeleeWeapon::StaticClass()));

		
			AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(Weapon);

			UE_LOG(LogTemp, Warning, TEXT("Cast result: %s"),
			   MeleeWeapon ? *MeleeWeapon->GetName() : TEXT("nullptr"));

		
			CharactersMeleeWeapon = MeleeWeapon;
			Combat->EquippedMeleeWeapon = MeleeWeapon;
			DisarmTwoHandedWeapon(MeleeWeapon);
			ArmTwoHandedWeapon(MeleeWeapon);
		}
	}
	if (EquippedWeapon != nullptr && Combat->EquippedWeapon != nullptr)
	{
		OverlappingWeapon = nullptr;
		OverlappingItem = nullptr;
	}
}

bool AFillainCharacter::IfPlayerAlreadyEquippedAnyWeapon()
{
	return OverlappingWeapon->WeaponState == EWeaponState::EWS_EquippedTwoHanded ||
		OverlappingWeapon->WeaponState == EWeaponState::EWS_EquippedOneHanded;
}

ARangedWeapon* AFillainCharacter::EquippedWeaponIsARangedWeapon()
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	UE_LOG(LogTemp, Warning, TEXT("CamParent=%s Arm=%.1f CamZ=%.1f CapZ=%.1f"),
	*GetNameSafe(FollowCamera ? FollowCamera->GetAttachParent() : nullptr),
	CameraBoom ? CameraBoom->TargetArmLength : -1.f,
	FollowCamera ? FollowCamera->GetComponentLocation().Z : -1.f,
	GetCapsuleComponent() ? GetCapsuleComponent()->GetComponentLocation().Z : -1.f);
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(DefaultFOV);
		bFOVLock = true;
		FOVLockTimeLeft = 0.75f; // hold for ~¾s; tweak if needed
	}
	ARangedWeapon* Ranged = Cast<ARangedWeapon>(EquippedWeapon);
	StartCamWatchdog(2.0f);
	if (Ranged == nullptr) return nullptr;
	else return Ranged;


}

AMeleeWeapon* AFillainCharacter::EquippedWeaponIsAMeleeWeapon()
{
	EQTRACE_MSG("OverlappingItem=%s OverlappingWeapon=%s",
		*GetNameSafe(OverlappingItem), *GetNameSafe(OverlappingWeapon));
	
	AMeleeWeapon* Melee = Cast<AMeleeWeapon>(EquippedWeapon);
	if (Melee == nullptr) return nullptr;
	else return Melee;
}

bool AFillainCharacter::WeaponIsUnclaimedFirearm(ARangedWeapon* Ranged)
{
	return Ranged->WeaponState == EWeaponState::EWS_Unclaimed &&
		Ranged->WeaponCategory == EWeaponCategory::EWC_Firearm;
}

bool AFillainCharacter::WeaponIsUnclaimedMeleeWeapon(AMeleeWeapon* Melee)
{
	return Melee->WeaponState == EWeaponState::EWS_Unclaimed;
}

bool AFillainCharacter::WeaponIsRanged()
{
	return IsValid(OverlappingWeapon) && OverlappingWeapon->IsA(ARangedWeapon::StaticClass());
}

bool AFillainCharacter::WeaponIsMelee()
{
	return IsValid(OverlappingWeapon) && OverlappingWeapon->IsA(AMeleeWeapon::StaticClass());
}

bool AFillainCharacter::ItemIsPickup()
{
	return IsValid(OverlappingItem) && OverlappingItem->IsA(AAmmoPickup::StaticClass());
}

void AFillainCharacter::SetAllWeaponEnumsForRanged()
{
	Combat->EquipWeapon(OverlappingWeapon);
	OverlappingWeapon = Combat->EquippedRangedWeapon;
	OverlappingWeapon = CharactersWeapon;
	CharactersWeapon = Combat->EquippedRangedWeapon;
	Combat->EquippedWeapon->SetEquippedWeaponState();
}

void AFillainCharacter::SetAllWeaponEnumsForMelee()
{
	Combat->EquipWeapon(OverlappingWeapon);
	OverlappingWeapon = Combat->EquippedMeleeWeapon;
	OverlappingWeapon = CharactersWeapon;
	CharactersWeapon = Combat->EquippedMeleeWeapon;
	OverlappingWeapon = nullptr;
	Combat->EquippedMeleeWeapon->SetEquippedMeleeWeaponState();
}

void AFillainCharacter::SetAllItemEnumsForPickup()
{
	AAmmoPickup* OverlappingAmmoPickup = Cast<AAmmoPickup>(OverlappingItem);
	Combat->PickupAmmo(Combat->RangedType, OverlappingAmmoPickup->AmountOfAmmoInside);
	OverlappingAmmoPickup->Destroy();
}

void AFillainCharacter::DisarmOneHandedWeapon(AMeleeWeapon* WeaponInHand)
{
	if (!IsValid(WeaponInHand))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndDisarmOneHandedWeapon: WeaponInHand is invalid."));
		return;
	}

	if (!IsValid(CharactersWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndDisarmOneHandedWeapon: CharactersWeapon is invalid."));
		return;
	}

	if (WeaponInHand == CharactersWeapon->OneHandedWeapon)
	{
		PlayArmDisarmMontage(FName("DisarmOneHanded"));

		if (IsValid(Combat) && IsValid(Combat->EquippedWeapon))
		{
			Combat->EquippedWeapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
			Combat->ActionState = EActionState::EAS_EquippingWeapon;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipAndDisarmOneHandedWeapon: Combat or Combat->EquippedWeapon is invalid."));
		}
	}
}

void AFillainCharacter::DisarmTwoHandedWeapon(AMeleeWeapon* WeaponInHand)
{
	if (!IsValid(WeaponInHand))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndDisarmTwoHandedWeapon: WeaponInHand is invalid."));
		return;
	}
	
	FName SectionName("DisarmTwoHanded");
	PlayArmDisarmMontage(SectionName);
	UE_LOG(LogTemp, Warning, TEXT("PLaying Montage Section: %s"), *SectionName.ToString());

	if (IsValid(Combat) && IsValid(Combat->EquippedWeapon))
	{
		Combat->EquippedWeapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
		Combat->ActionState = EActionState::EAS_EquippingWeapon;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndDisarmTwoHandedWeapon: Combat or Combat->EquippedWeapon is invalid."));
	}
}

void AFillainCharacter::ArmOneHandedWeapon(AMeleeWeapon* WeaponInHand)
{
	if (!IsValid(WeaponInHand))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndArmOneHandedWeapon: WeaponInHand is invalid."));
		return;
	}

	if (!IsValid(CharactersWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndArmOneHandedWeapon: CharactersWeapon is invalid."));
		return;
	}

	if (WeaponInHand == CharactersWeapon->OneHandedWeapon)
	{
		PlayArmDisarmMontage(FName("ArmOneHanded"));

		if (IsValid(Combat) && IsValid(Combat->EquippedWeapon))
		{
			Combat->EquippedWeapon->WeaponState = EWeaponState::EWS_EquippedOneHanded;
			Combat->ActionState = EActionState::EAS_EquippingWeapon;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipAndArmOneHandedWeapon: Combat or Combat->EquippedWeapon is invalid."));
		}
	}
}

void AFillainCharacter::ArmTwoHandedWeapon(AMeleeWeapon* WeaponInHand)
{
	if (!IsValid(WeaponInHand))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndArmTwoHandedWeapon: WeaponInHand is invalid."));
		return;
	}
	
	Combat->ActionState = EActionState::EAS_EquippingWeapon;
	PlayArmDisarmMontage(FName("ArmTwoHanded"));
	AttachWeaponToMeleeSocket();

	if (IsValid(Combat) && IsValid(Combat->EquippedWeapon))
	{
		Combat->EquippedWeapon->WeaponState = EWeaponState::EWS_EquippedTwoHanded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipAndArmTwoHandedWeapon: Combat or Combat->EquippedWeapon is invalid."));
	}
	ResetToFightAgain();
}

void AFillainCharacter::HitReactEnd()
{
	Combat->ActionState = EActionState::EAS_Unoccupied;
}

float AFillainCharacter::GetHitAssistPaddingCM()
{
	const UAbilitySystemComponent* AbilitySC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);
	if (!AbilitySC) return 0.f;

	const float VisionPct  = AbilitySC->GetNumericAttribute(UHAFAttributeSet::GetVisionAttribute()); // 0..100
	const float VisionFrac = FMath::Clamp(VisionPct * 0.01f, 0.f, 1.f);

	// 0..15 cm (~0..6 inches). Tweak to taste.
	return FMath::Lerp(0.f, 15.f, VisionFrac);
}

void AFillainCharacter::BindHiddenTreasureCapsuleHooksOnce()
{
	if (bAttrHooksBound || !ASC) return;

	ASC->GetGameplayAttributeValueChangeDelegate(UHAFAttributeSet::GetIntuitionAttribute())
		.AddUObject(this, &AFillainCharacter::OnIntuitionChanged);

	bAttrHooksBound = true;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (!IsValid(this) || !ASC) return;
				OnIntuitionChanged(FOnAttributeChangeData{});
			}));
	}
}

void AFillainCharacter::BindFillainCharacterCapsuleHooksOnce()
{
	if (bFillainCharacterCapsuleHooksBound) return;

	ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);
	HAFAttributeSet = GetHAFAttributeSet(); // (whichever getter you use)

	if (!ASC || !HAFAttributeSet) return;

	// Bind to all three stats — your original idea is good
	ASC->GetGameplayAttributeValueChangeDelegate(UHAFAttributeSet::GetAgilityAttribute())
		.AddUObject(this, &AFillainCharacter::OnFillainCharacterCapsuleScaleDriverChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(UHAFAttributeSet::GetFlexibilityAttribute())
		.AddUObject(this, &AFillainCharacter::OnFillainCharacterCapsuleScaleDriverChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(UHAFAttributeSet::GetDexterityAttribute())
		.AddUObject(this, &AFillainCharacter::OnFillainCharacterCapsuleScaleDriverChanged);

	// Kick an initial sync on whichever side we’re on
	OnFillainCharacterCapsuleScaleDriverChanged(FOnAttributeChangeData{});

	bFillainCharacterCapsuleHooksBound = true;
	// after BindFillainCharacterCapsuleHooksOnce() succeeds:
	if (HasAuthority() && !bDidInitialFillainCharacterCapsuleApply)
	{
		bDidInitialFillainCharacterCapsuleApply = true;
		// Give movement a frame to finish placing you
		GetWorldTimerManager().SetTimerForNextTick(this, &AFillainCharacter::Server_ApplyFillainCharacterCapsuleFromStats);
	}
}

	void AFillainCharacter::OnFillainCharacterCapsuleScaleDriverChanged(const FOnAttributeChangeData& Data)
	{
		Server_ApplyFillainCharacterCapsuleFromStats();
	}

bool AFillainCharacter::InitFillainCharacterCapsuleBaselinesIfNeeded()
{
	if (bFillainCharacterCapsuleBaselinesInit) return true;

	UCapsuleComponent* Cap = GetCapsuleComponent();
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (!Cap || !SkeletalMesh) return false;

	StandingUnscaledHalfHeight = FMath::Max(Cap->GetUnscaledCapsuleHalfHeight(), 1.f);
	StandingUnscaledRadius     = FMath::Max(Cap->GetUnscaledCapsuleRadius(),     1.f);
	StandingScaledHalfHeight   = FMath::Max(Cap->GetScaledCapsuleHalfHeight(),   1.f);
	StandingMeshRelZ           = SkeletalMesh->GetRelativeLocation().Z;

	bFillainCharacterCapsuleBaselinesInit = (StandingUnscaledHalfHeight > 1.f && StandingUnscaledRadius > 1.f);

	UE_LOG(LogTemp, Warning, TEXT("[Capsule] Baselines init: R=%.1f HH=%.1f (ScaledHH=%.1f)"),
		StandingUnscaledRadius, StandingUnscaledHalfHeight, StandingScaledHalfHeight);

	return bFillainCharacterCapsuleBaselinesInit;
}

void AFillainCharacter::OnIntuitionChanged(const FOnAttributeChangeData& Data)
{
	const float Pct  = ASC->GetNumericAttribute(UHAFAttributeSet::GetIntuitionAttribute());
	const float Frac = FMath::Clamp(Pct * 0.01f, 0.f, 1.f);

	for (auto* Scanner : TInlineComponentArray<UHiddenTreasureScannerComponent*>(this, true))
	{
		Scanner->SetIntuitionFraction(Frac);
	}
}

void AFillainCharacter::OnVisionChanged(const FOnAttributeChangeData& Data)
{
}

void AFillainCharacter::Server_ApplyFillainCharacterCapsuleFromStats_Implementation()
	{
		if (!ASC) return;

		const float Agility     = ASC->GetNumericAttribute(UHAFAttributeSet::GetAgilityAttribute());
		const float Flexibility = ASC->GetNumericAttribute(UHAFAttributeSet::GetFlexibilityAttribute());

		const float AgiPct  = FMath::Clamp(Agility,     0.f, 100.f) * 0.01f;
		const float FlexPct = FMath::Clamp(Flexibility, 0.f, 100.f) * 0.01f;

		float TargetUnscaledHalf   = StandingUnscaledHalfHeight * (1.f - AgiPct);
		float TargetUnscaledRadius = StandingUnscaledRadius     * (1.f - FlexPct);

		// Same clamps you already had
		TargetUnscaledHalf   = FMath::Clamp(TargetUnscaledHalf,   StandingUnscaledHalfHeight * 0.50f, StandingUnscaledHalfHeight);
		TargetUnscaledRadius = FMath::Clamp(TargetUnscaledRadius, StandingUnscaledRadius     * 0.60f, StandingUnscaledRadius);

		// Apply on the server (collision truth)
		ApplyFillainCharacterCapsuleSize_FeetPlanted(TargetUnscaledHalf, TargetUnscaledRadius);

		// Multicast so clients snap visually
		Multicast_ApplyFillainCharacterCapsuleSize(TargetUnscaledHalf, TargetUnscaledRadius);

		// Helpful logging
		UE_LOG(LogTemp, Warning, TEXT("[Capsule] Agi=%.1f Flex=%.1f => R=%.1f HH=%.1f"),
			Agility, Flexibility, TargetUnscaledRadius, TargetUnscaledHalf);
	}

	void AFillainCharacter::Multicast_ApplyFillainCharacterCapsuleSize_Implementation(float TargetUnscaledHalf, float TargetUnscaledRadius)
	{
		// Server already applied; clients mirror it
		if (!HasAuthority())
		{
			ApplyFillainCharacterCapsuleSize_FeetPlanted(TargetUnscaledHalf, TargetUnscaledRadius);
		}
	}

bool AFillainCharacter::PlayerHasSword()
{
	return Combat && Combat->bWieldingTheSword;
}

void AFillainCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (!bIsCrouched)
	{
		Crouch();
	}
}

void AFillainCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->Reload();
	}
}

bool AFillainCharacter::PlayerNotUsingRangedWeapons()
{
	return Combat && Combat->FightingStyle != EFightingStyle::EFS_Ranged && Combat->EquippedRangedWeapon == nullptr;
}

bool AFillainCharacter::PlayerUsingRangedWeapons()
{
	return Combat && Combat->FightingStyle == EFightingStyle::EFS_Ranged && Combat->EquippedRangedWeapon;;
}

void AFillainCharacter::AimButtonPressed()
{
	if (EquippedWeaponIsAMeleeWeapon()) return;
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AFillainCharacter::AimButtonReleased()
{
	if (EquippedWeaponIsAMeleeWeapon()) return; 
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

float AFillainCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void AFillainCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	CalculateAO_Pitch();
}

void AFillainCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

bool AFillainCharacter::SetTurningInPlaceEnum()
{
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return true;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	return false;
}

void AFillainCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedRangedWeapon == nullptr || Combat && Combat->FightingStyle != EFightingStyle::EFS_Ranged) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
}



bool AFillainCharacter::PlayerNotUsingRangedOrMeleeWeapons()
{
	return Combat && Combat->FightingStyle != EFightingStyle::EFS_Ranged || Combat->FightingStyle !=
		EFightingStyle::EFS_Melee;
}

bool AFillainCharacter::PlayerUsingMeleeWeapons()
{
	return Combat && Combat->FightingStyle == EFightingStyle::EFS_Melee;
}

void AFillainCharacter::AttackButtonReleased()
{
	if (PlayerNotUsingRangedOrMeleeWeapons()) return;
	if (bDisableGameplay)
	{
		bDisableGameplay = false;
	} 
	
	if (PlayerUsingRangedWeapons())
	{
		Combat->FireButtonPressed(false);
		ResetToFightAgain();
	}

	if (PlayerUsingMeleeWeapons())
	{
		Combat->FireButtonPressed(false);
		ResetToFightAgain();
	}
} 

void AFillainCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/* void AFillainCharacter::MulticastHit_Implementation()
{
	FName SectionName = FName("FromFront");
	PlayHitReactMontage(SectionName);
} */

void AFillainCharacter::HideCharacterIfCameraClose()
{
	if (!IsLocallyControlled() || !CameraBoom || !FollowCamera || !GetMesh()) return;

	const float Actual =
		(FollowCamera->GetComponentLocation() - CameraBoom->GetComponentLocation()).Size();

	// Decide desired state using hysteresis
	const float EnterT = FMath::Min(SelfOcclEnter, SelfOcclExit);
	const float ExitT  = FMath::Max(SelfOcclEnter, SelfOcclExit);

	const bool bDesiredOccluded = bSelfOccluded
		? (Actual < ExitT)      // stay occluded until we're comfortably past ExitT
		: (Actual < EnterT);    // only enter when we cross the lower band

	// Accumulate time in current state; only allow switch after hold time
	SelfOcclStateTime += GetWorld()->GetDeltaSeconds();
	if (bDesiredOccluded != bSelfOccluded && SelfOcclStateTime >= SelfOcclMinHold)
	{
		bSelfOccluded = bDesiredOccluded;
		SelfOcclStateTime = 0.f;

		// When occluded: stop boom collision + hide for owner
		CameraBoom->bDoCollisionTest = !bSelfOccluded;
		GetMesh()->SetOwnerNoSee(bSelfOccluded);

		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->SetOwnerNoSee(bSelfOccluded);
		}

		UE_LOG(LogTemp, Warning, TEXT("[SelfOccl] SWITCH  Actual=%.1f  Occluded=%d  BoomCollide=%d"),
			   Actual, bSelfOccluded, (int32)CameraBoom->bDoCollisionTest);
	}
	// else: hold current state; no spammy toggles
}

void AFillainCharacter::UpdateHUDAmmo()
{
	FillainPlayerController = FillainPlayerController == nullptr ? Cast<AFillainPlayerController>(Controller) : FillainPlayerController;

	if (FillainPlayerController && Combat && Combat->EquippedRangedWeapon)
	{
		FillainPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		FillainPlayerController->SetHUDWeaponAmmo(Combat->EquippedRangedWeapon->GetAmmo());
	}
}

void AFillainCharacter::PollInit()
{
	if (HAFPlayerState == nullptr)
	{
		HAFPlayerState = GetPlayerState<AHAFPlayerState>();
		if (HAFPlayerState)
		{
			OnPlayerStateInitialized();
			
			AHAFGameState* HAFGameState = Cast<AHAFGameState>(UGameplayStatics::GetGameState(this));

			if (HAFGameState && HAFGameState->TopScoringPlayers.Contains(HAFPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

void AFillainCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AFillainCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AFillainCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AFillainCharacter::SetOverlappingItem(APrePackagedPCPickupItem* Item)
{
	if (ATreasure* Treasure = Cast<ATreasure>(Item))
	{
		AddGoldAcquiredToTotalGold(Treasure);
	}
	if (AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(Item))
	{
		Combat->PickupAmmo(Combat->RangedType, AmmoPickup->AmountOfAmmoInside);
	}
	if (AHealthPickup* HealthPickup = Cast<AHealthPickup>(Item))
	{
		if (UAbilitySystemComponent* AbilitySC = GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySC->MakeOutgoingSpec(HealingEffect, 1.0f, AbilitySC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				AbilitySC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		Destroy();
	}
	if (AShieldPickup* ShieldPickup = Cast<AShieldPickup>(Item))
	{
		if (UAbilitySystemComponent* ASComponent = GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle = ASComponent->MakeOutgoingSpec(ShieldFortifyingEffect, 1.0f, ASComponent->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				ASComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		Destroy();
	}
	if (AStaminaPickup* StaminaPickup = Cast<AStaminaPickup>(Item))
	{
		if (UAbilitySystemComponent* AbilitySComponent = GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySComponent->MakeOutgoingSpec(StaminaRechargingEffect, 1.0f, AbilitySComponent->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				AbilitySComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		Destroy();
	}
	if (AMajixPickup* MajixPickup = Cast<AMajixPickup>(Item))
	{
		if (UAbilitySystemComponent* AbilitySystemC = GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemC->MakeOutgoingSpec(MajixSummoningEffect, 1.0f, AbilitySystemC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				AbilitySystemC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		Destroy();
	}
	if (ASoul* Soul = Cast<ASoul>(Item))
	{
		AddSoulsGatheredToTotalSouls(Soul);
	}
	else OverlappingItem = Item;
}

void AFillainCharacter::SetOverlappingWeapon(AWeaponBase* Weapon)
{

	OverlappingWeapon = Weapon;
	
	if (OverlappingWeapon && OverlappingWeapon != Weapon)
	{
		OverlappingWeapon->ShowPickupAndInfoWidgets(false);
	}
	
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupAndInfoWidgets(true);
	}
}

void AFillainCharacter::OnRep_OverlappingWeapon(AWeaponBase* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupAndInfoWidgets(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupAndInfoWidgets(false);
	}
}

void AFillainCharacter::OnRep_OverlappingItem(APrePackagedPCPickupItem* LastItem)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickupAndInfoWidgets(true);
	}
	if (LastItem)
	{
		LastItem->ShowPickupAndInfoWidgets(false);
	}
}

bool AFillainCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AFillainCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeaponBase* AFillainCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AFillainCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

EActionState AFillainCharacter::GetActionState() const
{
	if (Combat == nullptr) return EActionState::EAS_MAX;
	return Combat->ActionState;
}

EWeaponState AFillainCharacter::GetWeaponState() const
{
	if (Combat->EquippedWeapon == nullptr) return EWeaponState::EWS_MAX;
	return Combat->EquippedWeapon->WeaponState;
}

bool AFillainCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

ETeam AFillainCharacter::GetTeam()
{
	HAFPlayerState = HAFPlayerState == nullptr ? GetPlayerState<AHAFPlayerState>() : HAFPlayerState;
	if (HAFPlayerState == nullptr) return ETeam::ET_NoTeam;
	return HAFPlayerState->GetTeam();
}





/************************************************************************
**   I added the following functions to complete optional challenges   **
**   in the course, and they're proven to work correctly.			   **
************************************************************************/

void AFillainCharacter::SwitchWeapon(AWeaponBase* NewWeapon)
{
	if (NewWeapon && Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon = NewWeapon;
		// Update the HUD with the new weapon type
		AFillainPlayerController* PC = Cast<AFillainPlayerController>(GetFillainPlayerController());
		if (PC)
		{
			PC->SetHUDWeaponType(this);
		}
	}
}

APrePackagedPCPickupItem* AFillainCharacter::GetItemThatOverlaps(APrePackagedPCPickupItem* ItemThatOverlaps)
{
	AAmmoPickup* PickupThatOverlaps = Cast<AAmmoPickup>(ItemThatOverlaps);
	if (PickupThatOverlaps)
	{
		GetPickupThatOverlaps(PickupThatOverlaps);
	}
	return ItemThatOverlaps;
}

AWeaponBase* AFillainCharacter::GetWeaponThatOverlaps(AWeaponBase* WeaponThatOverlaps)
{
	return WeaponThatOverlaps;
}

AAmmoPickup* AFillainCharacter::GetPickupThatOverlaps(AAmmoPickup* PickupThatOverlaps)
{
	return PickupThatOverlaps;
}

void AFillainCharacter::CacheDamageParameters(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (CachedDamagedPawn == nullptr && CachedDamage == 0.0f && CachedDamageType == nullptr && CachedInstigatorController == nullptr && CachedCauser == nullptr)
	{
		CachedDamagedPawn = DamagedPawn;
		CachedDamage = Damage;
		CachedDamageType = DamageType;
		CachedInstigatorController = InstigatorController;
		CachedCauser = DamageCauser;
	}
}

void AFillainCharacter::ResetCachedDamageParameters()
{
	CachedDamageAmount = 0.f;
	CachedDamageEvent = FDamageEvent();
	CachedEventInstigator = nullptr;
	CachedDamageCauser = nullptr;
	CachedDamagedPawn = nullptr;
	CachedDamage = 0.0f;
	CachedDamageType = nullptr;
	CachedInstigatorController = nullptr;
	CachedCauser = nullptr;
}
void AFillainCharacter::InitASC()
{
	if (!AbilitySystemComponent) return;

	AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState<AHAFPlayerState>(), this);

	// Cache set for fast, safe access (UE 5.5.4)
	if (AbilitySystemComponent)
	{
		const UHAFAttributeSet* AsConst = AbilitySystemComponent->GetSet<UHAFAttributeSet>();
		HAFAS = const_cast<UHAFAttributeSet*>(AsConst); // caching; we won’t mutate
	}
	else
	{
		HAFAS = nullptr;
	}
}

void AFillainCharacter::InitializeAbilityActorInfo()
{
	AHAFPlayerState* FillainPlayerState = GetPlayerState<AHAFPlayerState>();
	check(FillainPlayerState);
	FillainPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(FillainPlayerState, this);
	Cast<UHAFAbilitySystemComponent>(FillainPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = FillainPlayerState->GetAbilitySystemComponent();
	AttributeSet = FillainPlayerState->GetAttributeSet();

	
	InitializeDefaultAttributes();
}

void AFillainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitFillainCharacterCapsuleBaselinesIfNeeded();
	// Input mapping (okay on listen server, ignored on dedicated)
	if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(HAFMappingContext, 0);
		}
	}

	AHAFPlayerState* PS = GetPlayerState<AHAFPlayerState>();
	check(PS);

	// Use the PS-owned ASC & AttributeSet (don’t call GetSet here)
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	HAFAttributeSet        = PS->GetHAFAttributeSet();

	// Owner = PlayerState, Avatar = Character
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AddCharacterAbilities();

	// Apply your init GameplayEffects on the SERVER
	if (HasAuthority())
	{
		ApplyEffectToSelf(DefaultPrimaryAttributes, 1);
		ApplyEffectToSelf(DefaultSecondaryAttributes, 1);
		ApplyEffectToSelf(DefaultVitalAttributes, 1);
		ApplyEffectToSelf(DefaultInvisibleAttributes, 1);
	}

	// Optional: quick pointer sanity
	UE_LOG(LogTemp, Warning, TEXT("[Character::PossessedBy] ASC=%s AS=%s"),
		*GetNameSafe(AbilitySystemComponent), *GetNameSafe(HAFAttributeSet));

	// Safe “post-init” log (no direct FGameplayAttributeData deref)
	auto SafeGet=[&](const FGameplayAttribute& A){return AbilitySystemComponent->GetNumericAttribute(A);};
	UE_LOG(LogTemp, Warning, TEXT("[SERVER?=%d] After InitializeDefaultAttributes: MaxHealth=%.3f Armor=%.3f Crit=%.3f"),
		HasAuthority(),
		SafeGet(HAFAttributeSet->GetMaxHealthAttribute()),
		SafeGet(HAFAttributeSet->GetArmorAttribute()),
		SafeGet(HAFAttributeSet->GetCriticalHitChanceAttribute()));

	// Ensure ASC is initialized (common place you do InitAbilityActorInfo)
	BindFillainCharacterCapsuleHooksOnce();        // server bind
	Server_ApplyFillainCharacterCapsuleFromStats(); // apply once on the server
	BindHiddenTreasureCapsuleHooksOnce();
}


void AFillainCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitFillainCharacterCapsuleBaselinesIfNeeded();
	AHAFPlayerState* PS = GetPlayerState<AHAFPlayerState>();
	if (!PS) return;

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	HAFAttributeSet        = PS->GetHAFAttributeSet();

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	UE_LOG(LogTemp, Warning, TEXT("[Character::OnRep_PlayerState] ASC=%s AS=%s"),
		*GetNameSafe(AbilitySystemComponent), *GetNameSafe(HAFAttributeSet));

	BindFillainCharacterCapsuleHooksOnce(); // client bind for UI responsiveness
	BindHiddenTreasureCapsuleHooksOnce();
}

void AFillainCharacter::ApplyFillainCharacterCapsuleSize_FeetPlanted(float TargetUnscaledHalf, float TargetUnscaledRadius)
{
	UCapsuleComponent* Cap = GetCapsuleComponent();
    USkeletalMeshComponent* Skeletal = GetMesh();
    if (!Cap || !Skeletal) return;

    // Ensure baselines are valid
    InitFillainCharacterCapsuleBaselinesIfNeeded();

    // Work on local copies (avoid re-declaring params)
    float Half   = TargetUnscaledHalf;
    float Radius = TargetUnscaledRadius;

    // Clamp vs. design limits
    const float MinHalf   = StandingUnscaledHalfHeight * 0.50f;
    const float MinRadius = StandingUnscaledRadius     * 0.60f;
    Half   = FMath::Clamp(Half,   MinHalf,   StandingUnscaledHalfHeight);
    Radius = FMath::Clamp(Radius, MinRadius, StandingUnscaledRadius);

    // Absolute safety floor (never feed tiny numbers to the capsule)
    Half   = FMath::Max(Half,   2.f);
    Radius = FMath::Max(Radius, 2.f);

    // BEFORE
    const float OldScaledHalf = Cap->GetScaledCapsuleHalfHeight();

    // Apply new size (only recompute overlaps after first-time init)
    Cap->SetCapsuleSize(Radius, Half, /*bUpdateOverlaps=*/ bFillainCharacterCapsuleInitialized);

    // AFTER
    const float NewScaledHalf = Cap->GetScaledCapsuleHalfHeight();

    UWorld* World = GetWorld();
    if (!bFillainCharacterCapsuleInitialized && World)
    {
        // First-time: sweep a capsule down to floor and teleport center so bottom sits on floor
        const FVector Up(0,0,1);
        const FVector Start = GetActorLocation() + Up * 20.f;
        const FVector End   = Start - Up * 4000.f;

        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(CapsuleSnapSweep), false, this);
        FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, Half);

        // Use the channel that your ground responds to; try ECC_WorldStatic first
        const ECollisionChannel FloorChannel = ECC_WorldStatic;

        const bool bHit = World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, FloorChannel, Shape, Params);

        const float PrevBottomZ = GetActorLocation().Z - OldScaledHalf;
        float FloorZ = -FLT_MAX;
        if (bHit && Hit.bBlockingHit)
        {
            const float ImpactBottomZ = Hit.Location.Z - NewScaledHalf;
            FloorZ = Hit.bStartPenetrating ? ImpactBottomZ + Hit.PenetrationDepth : ImpactBottomZ;
        }

        const float TargetBottomZ = (FloorZ > -FLT_MAX) ? FMath::Max(PrevBottomZ, FloorZ) : PrevBottomZ;
        FVector NewLoc = GetActorLocation();
        NewLoc.Z = TargetBottomZ + NewScaledHalf;
        SetActorLocation(NewLoc, /*bSweep=*/false, /*OutHit=*/nullptr, ETeleportType::TeleportPhysics);

        bFillainCharacterCapsuleInitialized = true;
    }
    else
    {
        // Subsequent resizes: keep feet planted with a swept vertical offset
        const float DeltaZ = (NewScaledHalf - OldScaledHalf);
        if (FMath::Abs(DeltaZ) > KINDA_SMALL_NUMBER)
        {
            AddActorWorldOffset(FVector(0,0,DeltaZ), /*bSweep=*/true);
        }
    }

    // Re-seat mesh from standing baseline
    FVector Rel = Skeletal->GetRelativeLocation();
    Rel.Z = StandingMeshRelZ + (StandingScaledHalfHeight - NewScaledHalf) + FeetToRootZOffset;
    Skeletal->SetRelativeLocation(Rel, false, nullptr, ETeleportType::ResetPhysics);

    // Refresh movement
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->bForceNextFloorCheck = true;
        if (Move->MovementMode == MOVE_None)
        {
            Move->SetMovementMode(MOVE_Walking);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[CapsuleApply] R=%.2f HH=%.2f NewScaledHalf=%.2f"),
        Radius, Half, NewScaledHalf);
}

 /******************************************************\
| **   The following were also added for challenges.  ** |
| **   They didn't do squat.			              ** |
 \******************************************************/

/* void AFillainCharacter::Restart()
{
	Super::Restart();
} 

void AFillainCharacter::OnFillainDying(AFillainCharacter* KillerFillain, AFillainCharacter* VictimFillain, AFillainPlayerController* InstigatorController)
{
	AHAFGameMode* HAFGameMode = GetWorld()->GetAuthGameMode<AHAFGameMode>();
	VictimCharacter = Cast<AFillainCharacter>(VictimFillain);
	VictimController = Cast<AFillainPlayerController>(VictimCharacter->GetController());
	AFillainPlayerController* KillerController = Cast<AFillainPlayerController>(InstigatorController);
	if (VictimCharacter && HAFGameMode && VictimController && KillerController)
	{
		HAFGameMode->PlayerEliminated(this, VictimController, KillerController);
		VictimController->SetHUDEliminationMessage(KillerController, VictimController);
		KillerController->SetHUDEliminationMessage(KillerController, VictimController);
	}
} */