// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/FillainCharacter.h"  
#include "GameFramework/SpringArmComponent.h"  
#include "Camera/CameraComponent.h"  
#include "GameFramework/CharacterMovementComponent.h"  
#include "Components/InputComponent.h"  
#include "EnhancedInputSubsystems.h"  
#include "EnhancedInputComponent.h"  
#include "Components/WidgetComponent.h"  
#include "GameFramework/PlayerState.h"  
#include "HUD/OverheadWidget.h"  
#include "Net/UnrealNetwork.h"  
#include "Weapons/WeaponBase.h"
#include "HAFComponents/CombatComponent.h"  
#include "HAFComponents/BuffComponent.h"  
#include "Components/CapsuleComponent.h"  
#include <Kismet/KismetMathLibrary.h>  
#include "Characters/FillainAnimInstance.h"  
#include "Characters/FillainFinalAnimInstance.h"
#include "HeroesAndFillains/HeroesAndFillains.h"  
#include "PlayerController/FillainPlayerController.h"  
#include "GameMode/HAFGameMode.h"  
#include "TimerManager.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"  
#include "Particles/ParticleSystemComponent.h"  
#include "PlayerState/HAFPlayerState.h"  
#include "Weapons/WeaponTypes.h"  
#include "Weapons/WeaponTypes.h"
#include "GameMode/LobbyGameMode.h"  
#include "Weapons/Ranged/Projectile.h"
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

#include "Characters/FillainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "HUD/OverheadWidget.h"
#include "Net/UnrealNetwork.h"
#include "HAFComponents/CombatComponent.h"
#include "HAFComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/KismetMathLibrary.h>

#include "IPersonaPreviewScene.h"
#include "NavigationSystem.h"
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
#include "GameMode/LobbyGameMode.h"
#include "Components/BoxComponent.h"
#include "HAFComponents/LagCompensationComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemies/EnemyBase.h"
#include "GameStates/HAFGameState.h"
#include "Pickups/AmmoPickup.h"
#include "PlayerStart/TeamPlayerStart.h"
#include "Weapons/Ranged/RangedWeapon.h"


AFillainCharacter::AFillainCharacter()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerCharacter);
	
	PrimaryActorTick.bCanEverTick = true;
	// SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 300.f;
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
	GetMesh()->SetCollisionObjectType(ECC_PlayerCharacter);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

void AFillainCharacter::BeginPlay()
{
	Super::BeginPlay();

	Combat = FindComponentByClass<UCombatComponent>();

	if (Combat)
	{
		Combat->SetCharacter(this); // this is AFillainCharacter*
	}

	if (HitReactMontage)
	{
		PlayHitReactMontage(FName("FromFront"));
	}
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No AnimInstance on mesh!"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ AnimInstance is valid: %s"), *AnimInstance->GetName());
	}
	if (!HitReactMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ HitReactMontage is null!"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ HitReactMontage is valid: %s"), *HitReactMontage->GetName());
	}
	float PlayResult = AnimInstance->Montage_Play(HitReactMontage, 1.0f);
	if (PlayResult <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Montage_Play returned 0. It failed to start playback."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Montage_Play returned: %f"), PlayResult);
	}
	UE_LOG(LogTemp, Warning, TEXT("IsAnyMontagePlaying: %s"), AnimInstance->IsAnyMontagePlaying() ? TEXT("TRUE") : TEXT("FALSE"));
	
	UE_LOG(LogTemp, Warning, TEXT("Player Capsule Collision ObjectType: %d"), GetCapsuleComponent()->GetCollisionObjectType());
	UE_LOG(LogTemp, Warning, TEXT("Player Capsule response to ECC_EnemyWeaponBox: %d"),
		GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_EnemyWeaponBox));
	
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
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AFillainCharacter::ReceiveDamage);
	}
	HideAttachedGrenade();

	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AFillainCharacter::OnArmDisarmMontageEnded);
	}
	
	Tags.Add(FName("EngageableTarget"));

	Tags.Add(FName("FillainCharacter"));

	UE_LOG(LogTemp, Warning, TEXT("Player Capsule ObjectType: %d"), GetCapsuleComponent()->GetCollisionObjectType());
	UE_LOG(LogTemp, Warning, TEXT("Player Capsule responds to ECC_EnemyWeaponBox: %d"), GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_EnemyWeaponBox));
	UE_LOG(LogTemp, Warning, TEXT("⚔️ Combat Component: %s"), Combat ? TEXT("VALID") : TEXT("NULL"));
}

void AFillainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(
	GetWorld(),
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation(),
	25.0f,
	12,
	FColor::Magenta,
	false,
	0.f
);

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

void AFillainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFillainCharacter, OverlappingItem, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFillainCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AFillainCharacter, Health);
	DOREPLIFETIME(AFillainCharacter, Shield);
	DOREPLIFETIME(AFillainCharacter, bDisableGameplay);
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


void AFillainCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (!IsValid(this)) return;

	if (AttributeComponent && AttributeComponent->IsCharacterAlive())
	{
		DirectionalHitReact(ImpactPoint);  // This plays the montage
	}

	UE_LOG(LogTemp, Warning, TEXT("💥 GetHit called. Damage = %f"), CachedDamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("Player Capsule response to ECC_EnemyWeaponBox: %d"),
		GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_EnemyWeaponBox));
}

float AFillainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("⚠️ Player took damage! New health: %f"), Health);

	if (Health <= 0.f)
	{
		Eliminate(true); // Or whatever function handles death
	}

	return DamageAmount;
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

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))

	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFillainCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AFillainCharacter::EquipButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AFillainCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AFillainCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AFillainCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AFillainCharacter::AttackButtonPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AFillainCharacter::AttackButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AFillainCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AFillainCharacter::GrenadeButtonPressed);
	}
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
			MeleeAttack();
		}
	}
}



void AFillainCharacter::MeleeAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(ArmDisarmMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Can't attack — ArmDisarmMontage still playing"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Trying to attack – ActionState = %d"), (int32)Combat->ActionState);
	UE_LOG(LogTemp, Warning, TEXT("🗡 MeleeAttack() called"));

	if (CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("🟢 Passed CanAttack()"));

		PlayMeleeAttackMontage();
		Combat->ActionState = EActionState::EAS_MeleeAttacking;
		
		// Don’t set ActionState back to Unoccupied here anymore. Let the montage handle it via Notify.
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
	if (Combat)
	{
		Combat->ActionState = EActionState::EAS_Unoccupied;
		UE_LOG(LogTemp, Warning, TEXT("✅ AttackEnd() triggered via AnimNotify"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Combat is NULL in AttackEnd()"));
	}
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
	UE_LOG(LogTemp, Warning, TEXT("CanAttack: ActionState = %d | FightingStyle = %d"),
	(int32)Combat->ActionState,
	(int32)Combat->FightingStyle);
	return IfPlayerIsReadyToFightAgain() && IfPlayerHasEquippedAWeapon();
}

void AFillainCharacter::PlayHitReactMontage(const FName& SectionName)
{
	Super::PlayHitReactMontage(SectionName);
}	

void AFillainCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void AFillainCharacter::CalculateShieldDamage(float Damage, float& DamageToHealth)
{
	DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
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
		ControllerOfVictim->SetHUDEliminationMessage(KillerController, ControllerOfVictim);
		KillerController->SetHUDEliminationMessage(KillerController, ControllerOfVictim);
	}
}

void AFillainCharacter::ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedPawn, Damage, DamageType, InstigatorController, DamageCauser);
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
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Deadzone threshold (adjust as needed)
	const float DeadzoneThreshold = 0.4f;

	// Check magnitude of the stick input
	if (LookAxisVector.Size() < DeadzoneThreshold)
	{
		return; // Ignore small drift input
	}

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

	//UE_LOG(LogTemp, Warning, TEXT("LookAxisVector: %s"), *LookAxisVector.ToString());

}

void AFillainCharacter::EquipButtonPressed()
{
	UE_LOG(LogTemp, Error, TEXT("🎯 EquipButtonPressed() triggered"));

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

	AWeaponBase* WeaponToEquip = Cast<AWeaponBase>(OverlappingWeapon ? OverlappingWeapon : OverlappingItem);
	if (IsValid(WeaponToEquip))
	{
		ServerEquipButtonPressed(WeaponToEquip);
		SetOverlappingItem(nullptr);
		SetOverlappingWeapon(nullptr);
	}
	if (!IsValid(WeaponToEquip))
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ No valid weapon to equip"));
		return;
	}

	// ✅ Send to server
	ServerEquipButtonPressed(OverlappingWeapon);

	// ✅ Clean up local overlap (Dark Souls style)
	SetOverlappingItem(nullptr);
	SetOverlappingWeapon(nullptr);

	UE_LOG(LogTemp, Warning, TEXT("✅ Cleared overlapping references after successful equip"));
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
	if (!Combat || !Weapon) return;

	if (Combat->ActionState == EActionState::EAS_EquippingWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Already equipping — ignoring input"));
		return;
	}

	// Disarm/arm toggle
	if (EquippedWeapon && EquippedWeapon->ItemState == EItemState::EIS_Equipped)
	{
		// Arm/disarm logic (as you already have it)...
		// This is good — no major changes needed here
		return;
	}

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
		ToggleArmingAndDisarming();
	}

	if (Weapon && Weapon->ItemState == EItemState::EIS_Equipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ Server equipped %s"), *Weapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Weapon not marked equipped after Equip call"));
	}
}


void AFillainCharacter::EquipWeapon(AWeaponBase* Weapon)
{
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
}

void AFillainCharacter::EquipOneHandedRangedWeapon(AWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("✅ Equipping %s, Current ActionState: %d"), *Weapon->GetName(), (int32)Combat->ActionState);
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
	UE_LOG(LogTemp, Warning, TEXT("✅ Equipping %s, Current ActionState: %d"), *Weapon->GetName(), (int32)Combat->ActionState);
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
	UE_LOG(LogTemp, Warning, TEXT("✅ Equipping %s, Current ActionState: %d"), *Weapon->GetName(), (int32)Combat->ActionState);
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
	UE_LOG(LogTemp, Warning, TEXT("👉 Entering EquipTwoHandedMeleeWeapon"));
	UE_LOG(LogTemp, Warning, TEXT("✅ Equipping %s, Current ActionState: %d"), *Weapon->GetName(), (int32)Combat->ActionState);
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
	ARangedWeapon* Ranged = Cast<ARangedWeapon>(EquippedWeapon);
	if (Ranged == nullptr) return nullptr;
	else return Ranged;
}

AMeleeWeapon* AFillainCharacter::EquippedWeaponIsAMeleeWeapon()
{
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

bool AFillainCharacter::PlayerHasSword()
{
	return Combat && Combat->bWieldingTheSword;
}

void AFillainCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
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

void AFillainCharacter::MulticastHit_Implementation()
{
	FName SectionName = FName("FromFront");
	PlayHitReactMontage(SectionName);
}

void AFillainCharacter::HideCharacterIfCameraClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AFillainCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	UpdateHUDShield();
	if (Health < LastHealth)
	{
		FName SectionName("FromFront");
		PlayHitReactMontage(SectionName);
	}
}

void AFillainCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		FName SectionName("FromFront");
		PlayHitReactMontage(SectionName);
	}
}

void AFillainCharacter::UpdateHUDHealth()
{
	FillainPlayerController = FillainPlayerController == nullptr ? Cast<AFillainPlayerController>(Controller) : FillainPlayerController;

	if (FillainPlayerController)
	{
		FillainPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AFillainCharacter::UpdateHUDShield()
{
	FillainPlayerController = FillainPlayerController == nullptr ? Cast<AFillainPlayerController>(Controller) : FillainPlayerController;

	if (FillainPlayerController)
	{
		FillainPlayerController->SetHUDShield(Shield, MaxShield);
	}
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

void AFillainCharacter::SetOverlappingItem(AItem* Item)
{
	UE_LOG(LogTemp, Error, TEXT("SetOverlappingItem called — Item: %s | Current OverlappingItem: %s"),
	Item ? *Item->GetName() : TEXT("nullptr"),
	OverlappingItem ? *OverlappingItem->GetName() : TEXT("nullptr"));
	
	UE_LOG(LogTemp, Warning, TEXT("SetOverlappingItem called with: %s"), Item ? *Item->GetName() : TEXT("nullptr"));

	if (Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemState = %s"), *UEnum::GetValueAsString(Item->ItemState));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemState = nullptr — no item to report"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Setting OverlappingItem: %s"), Item ? *Item->GetName() : TEXT("nullptr"));
	
	// Prevent clearing equipped weapon via null update
	if (!IsValid(Item))
	{
		if (OverlappingItem && OverlappingItem->ItemState == EItemState::EIS_Equipped)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignoring null OverlappingItem because item is equipped"));
			return;
		}

		if (EquippedWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignoring nullptr because player is armed"));
			return;
		}

		OverlappingItem = nullptr;
		OverlappingWeapon = nullptr;
		return;
	}

	OverlappingItem = Item;
	UE_LOG(LogTemp, Warning, TEXT("✅ Assigned OverlappingItem to: %s"), *OverlappingItem->GetName());

	if (Item->IsA(AWeaponBase::StaticClass()))
	{
		AWeaponBase* Weapon = Cast<AWeaponBase>(Item);
		OverlappingWeapon = Weapon;
		UE_LOG(LogTemp, Warning, TEXT("OverlappingWeapon is now: %s"), *Weapon->GetName());
	}
	else
	{
		OverlappingWeapon = nullptr;
	}

	// Hide pickup widget if it's an ammo pickup (non-weapon)
	if (OverlappingItem->IsA(AAmmoPickup::StaticClass()))
	{
		OverlappingItem->ShowPickupAndInfoWidgets(false);
	}

	// Show widgets for valid locally controlled weapons
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupAndInfoWidgets(true);
	}
}

void AFillainCharacter::SetOverlappingWeapon(AWeaponBase* Weapon)
{
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ SetOverlappingWeapon called with nullptr"));
		return;
	}

	OverlappingWeapon = Weapon;
	UE_LOG(LogTemp, Warning, TEXT("✅ OverlappingWeapon set to: %s"), *Weapon->GetName());
	
	if (OverlappingWeapon && OverlappingWeapon != Weapon)
	{
		OverlappingWeapon->ShowPickupAndInfoWidgets(false);
	}

	OverlappingWeapon = Weapon;

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

void AFillainCharacter::OnRep_OverlappingItem(AItem* LastItem)
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

AItem* AFillainCharacter::GetItemThatOverlaps(AItem* ItemThatOverlaps)
{
	AWeaponBase* WeaponThatOverlaps = Cast<AWeaponBase>(ItemThatOverlaps);
	AAmmoPickup* PickupThatOverlaps = Cast<AAmmoPickup>(ItemThatOverlaps);
	if (WeaponThatOverlaps == nullptr && PickupThatOverlaps == nullptr) return nullptr;
	if (WeaponThatOverlaps)
	{
		GetWeaponThatOverlaps(WeaponThatOverlaps);
	}
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

AFillainPlayerController* AFillainCharacter::GetFillainPlayerController()
{
	AFillainPlayerController* FillainController = Cast<AFillainPlayerController>(GetController());
	return FillainController;
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

void AFillainCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ShowPlayerName();
}

void AFillainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Ensure FillainPlayerController is initialized

	if (AFillainPlayerController* NewFillainController = Cast<AFillainPlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewFillainController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(HAFMappingContext, 0);
		}
		
		ShowPlayerName();
	}
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