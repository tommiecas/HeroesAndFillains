// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"  
#include "HAFComponents/CombatComponent.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeroesAndFillains/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAFComponents/AttributeComponent.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"  
#include "Components/InputComponent.h"  
#include "EnhancedInputSubsystems.h"  
#include "EnhancedInputComponent.h"  
#include "Components/WidgetComponent.h"  
#include "GameFramework/PlayerState.h"  
#include "UI/OverheadWidget.h"  
#include "Net/UnrealNetwork.h"  
#include "Weapons/WeaponBase.h"
#include "HAFComponents/CombatComponent.h"  
#include "HAFComponents/BuffComponent.h"  
#include "Components/CapsuleComponent.h"  
#include <Kismet/KismetMathLibrary.h>
#include "ASync/ASync.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"

#include "K2Node_CallFunction.h"
#include "Characters/FillainAnimInstance.h"  
#include "Characters/FillainFinalAnimInstance.h"
#include "HeroesAndFillains/HeroesAndFillains.h"  
#include "PlayerController/FillainPlayerController.h"  
#include "GameMode/HaFGameMode.h"  
#include "TimerManager.h"  
#include "Kismet/GameplayStatics.h"  
#include "Sound/SoundCue.h"  
#include "Particles/ParticleSystemComponent.h"  
#include "PlayerState/HAFPlayerState.h"  
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"  
#include "HeroesAndFillains/HeroesAndFillainsTypes/CharacterTypes.h"
#include "GameMode/LobbyGameMode.h"  
#include "Weapons/Ranged/Projectile.h"
#include "Components/BoxComponent.h"  
#include "HAFComponents/LagCompensationComponent.h"  
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "Enemies/EnemyBase.h"
#include "GameStates/HAFGameState.h"  
#include "PlayerStart/TeamPlayerStart.h"
#include "GameFramework/Actor.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/FillainFinalAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "PlayerController/FillainPlayerController.h"
#include "GameFramework/Controller.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "ModifiedMagnitudeCalculations/MMC_Agility.h"
#include "ModifiedMagnitudeCalculations/MMC_DexterityAgilityFlexibility.h"
#include "ModifiedMagnitudeCalculations/MMC_Intuition.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxHealth.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxMajix.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxShield.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxStamina.h"
#include "Pickups/HealthPickup.h"
#include "Pickups/MajixPickup.h"
#include "Pickups/Pickup.h"
#include "Pickups/ShieldPickup.h"
#include "Pickups/StaminaPickup.h"
#include "Components/CapsuleComponent.h"
#include "HAFGameplayTags.h"
#include "GameplayEffectTypes.h" // defines FGameplayEffectSpecHandle::Data and FGameplayEffectSpec


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GetCapsuleComponent());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetupAttachment(RootComponent);
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, MeleeAttackMontage);
	DOREPLIFETIME(ABaseCharacter, MeleeAttackMontages);
	DOREPLIFETIME(ABaseCharacter, MajixAttackMontage);
	DOREPLIFETIME(ABaseCharacter, MajixAttackMontages);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!bRequiresASC) return;

	// Do NOT crash here. Just try to initialize whatever model this character uses.
	if (AbilitySystemComponent)
	{
		// Pawn-owned ASC (NPCs)
		SafeInitASC_ForPawnOwner();
	}
	// For player-controlled pawns with ASC on PlayerState, ASC will be wired in PossessedBy/OnRep_PlayerState.
	
	// Do NOT hard-assert here. In PIE, players using ASC-on-PlayerState often hit BeginPlay
	// before ASC is assigned. Log and continue; we'll finish wiring in PossessedBy/OnRep.
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: AbilitySystemComponent not yet assigned in BeginPlay; will initialize later via PossessedBy/OnRep_PlayerState."), *GetName());
	}

	// --- AI enemies (ASC lives on the Pawn): safe to Init here if ASC exists ---
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(this))
	{
		if (AbilitySystemComponent && !AbilitySystemComponent->GetAvatarActor())
		{
			// Owner = this (pawn), Avatar = this (pawn) for pawn-owned ASCs
			AbilitySystemComponent->InitAbilityActorInfo(/*OwnerActor=*/this, /*AvatarActor=*/this);
		}
	}

	// --- Player pawn (AFillainCharacter): DO NOT call InitAbilityActorInfo in BeginPlay ---
	// Players typically keep the ASC on the PlayerState. That init belongs in PossessedBy/OnRep_PlayerState.
	if (AFillainCharacter* AFC = Cast<AFillainCharacter>(this))
	{
		// (Unrelated to ASC) Input tags are fine to set up here.
		AttackTags.Reset();
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.LeftMouseButtonOrGamepadShoulder")));
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.RightMouseButtonOrGamepadShoulder")));
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.1OrDPadUp")));
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.2OrDPadDown")));
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.3OrDPadLeft")));
		AttackTags.Add(FGameplayTag::RequestGameplayTag(FName("InputTag.4OrDPadRight")));
	}
}

void ABaseCharacter::SafeInitASC_ForPawnOwner()
{
	if (!AbilitySystemComponent) return;

	if (!AbilitySystemComponent->GetAvatarActor())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// NEW: cache your AttributeSet (safe if called multiple times)
	AttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetSet<UAttributeSet>());
}

void ABaseCharacter::SafeInitASC_FromPlayerState()
{
	if (APlayerState* PS = GetPlayerState())
	{
		if (IAbilitySystemInterface* PSI = Cast<IAbilitySystemInterface>(PS))
		{
			if (UAbilitySystemComponent* PlayerASC = PSI->GetAbilitySystemComponent())
			{
				AbilitySystemComponent = PlayerASC;
				if (!PlayerASC->GetAvatarActor())
				{
					PlayerASC->InitAbilityActorInfo(PS, this);
				}
				// NEW:
				AttributeSet = const_cast<UAttributeSet*>(PlayerASC->GetSet<UAttributeSet>());
			}
		}
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!bRequiresASC) return;

	SafeInitASC_FromPlayerState();
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!bRequiresASC) return;

	SafeInitASC_FromPlayerState();
}

float ABaseCharacter::GetCurrentHealth() const
{
	if (!AbilitySystemComponent) return 0.f;

	// Fast path if we cached the set (optional)
	if (AttributeSet)
	{
		// If your AttributeSet exposes a direct getter, either is fine:
		// return HAFAttributeSet->GetHealth();
		const FGameplayAttribute HealthAttr = UHAFAttributeSet::GetHealthAttribute();
		return AbilitySystemComponent->GetNumericAttribute(HealthAttr);
	}

	// Safe path even without cached set
	const FGameplayAttribute HealthAttr = UHAFAttributeSet::GetHealthAttribute();
	return AbilitySystemComponent->GetNumericAttribute(HealthAttr);
}

bool ABaseCharacter::IsDying() const
{
	return GetCurrentHealth() <= KINDA_SMALL_NUMBER;
}

bool ABaseCharacter::IsAlive() const
{
	return GetCurrentHealth() > KINDA_SMALL_NUMBER;
}

bool ABaseCharacter::IsDead() const
{
	return !IsAlive();
}

void ABaseCharacter::InitializeAbilityActorInfo()
{
	
}

void ABaseCharacter::ConsumeDodgeStamina()
{
	if (!AbilitySystemComponent || !GE_DodgeStaminaCost) return;

	const float DodgeCost = static_cast<float>(GetDodgeCost());
	if (DodgeCost <= 0.f) return;

	const float CurStamina =
		AbilitySystemComponent->GetNumericAttribute(UHAFAttributeSet::GetStaminaAttribute());
	if (CurStamina < DodgeCost) return;

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_DodgeStaminaCost, 1.f, Ctx);
	if (!SpecHandle.IsValid()) return;

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!ensure(Spec)) return;

	Spec->SetSetByCallerMagnitude(TAG_SetByCaller_StaminaCost, -DodgeCost);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
}

void ABaseCharacter::Die()
{
	if (Weapon) Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

void ABaseCharacter::MulticastHandleDeath_Implementation()
{
	if (Weapon)
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetMesh()->SetCollisionObjectType(ECC_WorldStatic);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Dissolve();
}

void ABaseCharacter::Dissolve()
{
	if (bIsCharacterDead) return;
	
}
void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// Early exit if actor is pending kill or invalid
	if (!IsValid(this)) return;

	// Mark this character as having been hit
	WasBaseCharacterHit = true;

	// ❌ Do NOT call TakeDamage here, since GameplayEffects already applied health changes

	// Handle hit reaction or death
	if (GetCurrentHealth() > KINDA_SMALL_NUMBER)
	{
		if (IsValid(HitReactMontage) && IsValid(Hitter))
		{
			DirectionalHitReact(Hitter->GetActorLocation());
		}
	}
	else
	{
		Die();
	}

	// Play audiovisual feedback
	PlayHitSound(ImpactPoint);
	SpawnHitSpecialEffects(ImpactPoint);
}

FDirectionalHitResult ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	FDirectionalHitResult Result;

	const FVector Forward = GetActorForwardVector();
	const FVector ActorLocation = GetActorLocation();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, ActorLocation.Z);
	const FVector ToHit = (ImpactLowered - ActorLocation).GetSafeNormal();

	Theta = FMath::RadiansToDegrees(
		FMath::Acos(FVector::DotProduct(Forward, ToHit))
	);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	if (Theta >= -45.f && Theta < 45.f)        Result.bFromFront = true;
	else if (Theta >= -135.f && Theta < -45.f) Result.bFromLeft  = true;
	else if (Theta >= 45.f && Theta < 135.f)   Result.bFromRight = true;
	else                                       Result.bFromBack  = true;

	return Result;
}

void ABaseCharacter::MeleeAttack()
{
	if (this->IsA(AEnemyBase::StaticClass()) && CombatTarget && CombatTarget->ActorHasTag(FName("Enemy"))) return;
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::MajixAttack()
{
	if (this->IsA(AEnemyBase::StaticClass()) && CombatTarget && CombatTarget->ActorHasTag(FName("Enemy"))) return;
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::CharacterDies()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterDies() called for: %s"), *GetName());

	if (bIsCharacterDead) return;
	bIsCharacterDead = true;
	
	if (AEnemyBase* DeadEnemy = Cast<AEnemyBase>(this))
	{
		Tags.Add(FName("Dead"));
		Die();
	}
	if (AFillainCharacter* DeadFillain = Cast<AFillainCharacter>(this))
	{
		HAFGameMode = HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : HAFGameMode;
		if (HAFGameMode && DeadFillain)
		{
			DeadFillain->MulticastEliminate(false);
			AFillainPlayerController* VictimPlayerController = Cast<AFillainPlayerController>(DeadFillain->GetFillainPlayerController());
			if (KillerPlayerController) KillerPlayerController->InitializeHUDEliminationMessage(KillerPlayerController, VictimPlayerController, CachedInstigatorController);
			if (!KillerPlayerController) VictimPlayerController->InitializeHUDEliminationMessage(KillerPlayerController, VictimPlayerController, CachedInstigatorController);
		}
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& Section)
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HitReactMontage) return;
	/*
		// Skip if not allowed
		if (!bCanReact)
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ Skipping HitReact: bCanReact is false"));
			return;
		}
	
		bCanReact = false;
		GetWorld()->GetTimerManager().SetTimer(HitReactTimer, this, &ABaseCharacter::ResetHitReact, 0.25f, false);
	
		// Force reset via Reset section BEFORE jumping to section
		const FString DirectionName = Section.ToString().Replace(TEXT("From"), TEXT(""));
		const FName ResetSection = FName(*FString::Printf(TEXT("Reset%s"), *DirectionName));
		if (HitReactMontage->IsValidSectionName(ResetSection))
		{
			UE_LOG(LogTemp, Warning, TEXT("🔁 Forcing reset by jumping to: %s"), *ResetSection.ToString());
	
			// Step 1: Play the montage if needed */
	AnimInstance->Montage_Play(HitReactMontage);
	/*
			// Step 2: Set next section from Reset_Section back to actual HitReact section
			AnimInstance->Montage_SetNextSection(ResetSection, Section, HitReactMontage);
			UE_LOG(LogTemp, Warning, TEXT("➡️ Set next section after %s to %s"), *ResetSection.ToString(), *Section.ToString());
	
			// Step 3: Jump to the Reset section now */
	AnimInstance->Montage_JumpToSection(Section, HitReactMontage);
	WasBaseCharacterHit = false;
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (IsValid(HitSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitSpecialEffects(const FVector& ImpactPoint)
{
	if (IsValid(HitReactParticles))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitReactParticles, ImpactPoint);
	}
	else if (IsValid(HitReactSystem))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitReactSystem, ImpactPoint, GetActorRotation());
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComponent)
	{
		AttributeComponent->CharactersReceiveMeleeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	AActor* InstigatorActor = Cast<AActor>(EventInstigator);
	MaybeTriggerCharm(this, InstigatorActor);
}

void ABaseCharacter::MaybeTriggerCharm(AActor* DamagedActor, AActor* DamageInstigator)
{
	// Quick guards
	if (!DamagedActor || !DamageInstigator) return;
	if (!HasAuthority()) return;

	// If we're not on the game thread, hop back and re-run safely.
	if (!IsInGameThread())
	{
		TWeakObjectPtr<ABaseCharacter> WeakThis(this);
		TWeakObjectPtr<AActor> WeakDamaged(DamagedActor);
		TWeakObjectPtr<AActor> WeakInstigator(DamageInstigator);

		AsyncTask(ENamedThreads::GameThread, [WeakThis, WeakDamaged, WeakInstigator]()
		{
			if (!WeakThis.IsValid() || !WeakDamaged.IsValid() || !WeakInstigator.IsValid()) return;
			WeakThis->MaybeTriggerCharm(WeakDamaged.Get(), WeakInstigator.Get());
		});
		return;
	}

	// --- From here, game thread only ---

	const UAttributeSet* AS = GetAttributeSet();
	if (!AS)
	{
		UE_LOG(LogTemp, Warning, TEXT("HAFAttributeSet is null on %s"), *GetNameSafe(this));
		return;
	}

	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	if (HAFAttributeSet) const float CurrentCharm = HAFAttributeSet->GetCharm();  // 0..100
	if (HAFAttributeSet->GetCharm() <= 0.f) return;

	const float Chance = FMath::Clamp(HAFAttributeSet->GetCharm(), 0.f, 100.f) * 0.01f;
	const float Roll   = FMath::FRand();

	if (Roll > Chance) return;

	if (AEnemyBase* EnemyVictim = Cast<AEnemyBase>(DamagedActor))
	{
		EnemyVictim->TriggerCharm(DamageInstigator);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	const int32 SectionIndex = Montage->GetSectionIndex(SectionName);
	if (!Montage || SectionName.IsNone() || !AnimInstance || SectionIndex == INDEX_NONE)  return;
	
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(SectionName, Montage);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> SectionNames)
{
	if (!Montage || SectionNames.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	const FName& SelectedSection = SectionNames[Selection];

	int32 SectionIndex = Montage->GetSectionIndex(SelectedSection);
	if (SectionIndex == INDEX_NONE) return -1;;
	
	PlayMontageSection(Montage, SelectedSection);
	return Selection;
}

void ABaseCharacter::PlayAttackMontage(const FGameplayTag& InputTag)
{
	

}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);;
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::StopAllMontages(float BlendOutTime)
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->StopAllMontages(BlendOutTime);
		UE_LOG(LogTemp, Warning, TEXT("🛑 Stopped all montages on %s"), *GetName());
	}
}

void ABaseCharacter::StopMontage(UAnimMontage* Montage)
{
	UAnimMontage* MontageToStop = Montage ? Montage : CurrentAttackMontage;

	if (MontageToStop && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, MontageToStop);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToAttacker = (Location - CombatTargetLocation).GetSafeNormal();;
	TargetToAttacker *= WarpTargetDistance;

	return CombatTargetLocation + TargetToAttacker; 	
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ABaseCharacter::GetSpellCastersSocketLocation()
{
	check (GetMesh());
	return GetMesh()->GetSocketLocation(SpellCastersSocketName);
}

bool ABaseCharacter::CanAttack()
{
	return true;
}

bool ABaseCharacter::IsCharacterAlive() const
{
	return GetCurrentHealth() > KINDA_SMALL_NUMBER;
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

float ABaseCharacter::SafeGetNumeric(const UAbilitySystemComponent* ASC, const UHAFAttributeSet* AS,
	const FGameplayAttribute& Attr)
{
	return (ASC && AS) ? ASC->GetNumericAttribute(Attr) : NAN;
}

float ABaseCharacter::SafeGet(const UAbilitySystemComponent* ASC, const UHAFAttributeSet* AS, const FGameplayAttribute& Attr)
{
    return (ASC && AS) ? ASC->GetNumericAttribute(Attr) : NAN;
}

void ABaseCharacter::ApplyStartupEffects() const
{
	 if (!HasAuthority() || !AbilitySystemComponent) return;

    auto ApplyGE = [this](TSubclassOf<UGameplayEffect> GEClass)
    {
    	UHAFAttributeSet* HAFAttriSet = Cast<UHAFAttributeSet>(AttributeSet);
        if (!GEClass) return;

    	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();

    	Ctx.AddSourceObject(this);

    	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GEClass, 1.f, Ctx);

    	if (Spec.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
    };

	UHAFAttributeSet* HAFAS = Cast<UHAFAttributeSet>(AttributeSet);
    ApplyGE(DefaultPrimaryAttributes);

	ApplyGE(DefaultSecondaryAttributes);

	ApplyGE(DefaultResistanceAttributes);
	UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());

	ApplyGE(DefaultVitalAttributes);
	UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());

	ApplyGE(DefaultInvisibleAttributes);

	UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());
	
    if (HasAuthority() && HAFAS)   // ← changed from AttributeSet
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] After ApplyStartupEffects: Armor=%f ArmorPenetration=%f BlockChance=%f CriticalHitChance=%f CriticalHitDamage=%f CriticalHitResistance=%f Agility=%f Flexibility=%f Purity=%f Corruptibility=%f Intuition=%f Vision=%f Charm=%f HealthRegeneration=%f ShieldRegeneration=%f StaminaRegeneration=%f MajixRegeneration=%f MaxHealth=%f MaxShield=%f MaxStamina=%f MaxMajix=%f"),
            HAFAS->GetArmor(),
            HAFAS->GetArmorPenetration(),
            HAFAS->GetBlockChance(),
            HAFAS->GetCriticalHitChance(),
            HAFAS->GetCriticalHitDamage(),
            HAFAS->GetCriticalHitResistance(),
            HAFAS->GetAgility(),
            HAFAS->GetFlexibility(),
            HAFAS->GetPurity(),
            HAFAS->GetCorruptibility(),
            HAFAS->GetIntuition(),
            HAFAS->GetVision(),
            HAFAS->GetCharm(),
            HAFAS->GetHealthRegeneration(),
            HAFAS->GetShieldRegeneration(),
            HAFAS->GetStaminaRegeneration(),
            HAFAS->GetMajixRegeneration(),
            HAFAS->GetMaxHealth(),
            HAFAS->GetMaxShield(),
            HAFAS->GetMaxStamina(),
            HAFAS->GetMaxMajix());
    }
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultResistanceAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	ApplyEffectToSelf(DefaultInvisibleAttributes, 1.f);
	
	LogSecondaries_Server();
}

void ABaseCharacter::LogSecondaries_Server() const
{
    const UAbilitySystemComponent* ASC = AbilitySystemComponent;
    const UHAFAttributeSet* AS = Cast<UHAFAttributeSet>(AttributeSet);

    /* UE_LOG(LogTemp, Warning, TEXT("[SERVER] After InitializeDefaultAttributes: "
        "Armor=%f ArmorPenetration=%f BlockChance=%f CriticalHitChance=%f CriticalHitDamage=%f CriticalHitResistance=%f "
        "Agility=%f Flexibility=%f Purity=%f Corruptibility=%f Intuition=%f Vision=%f Charm=%f "
        "HealthRegeneration=%f ShieldRegeneration=%f StaminaRegeneration=%f MajixRegeneration=%f "
        "MaxHealth=%f MaxShield=%f MaxStamina=%f MaxMajix=%f"),
        SafeGetNumeric(ASC, AS, AS->GetArmorAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetArmorPenetrationAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetBlockChanceAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetCriticalHitChanceAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetCriticalHitDamageAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetCriticalHitResistanceAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetAgilityAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetFlexibilityAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetPurityAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetCorruptibilityAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetIntuitionAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetVisionAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetCharmAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetHealthRegenerationAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetShieldRegenerationAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetStaminaRegenerationAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetMajixRegenerationAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetMaxHealthAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetMaxShieldAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetMaxStaminaAttribute()),
        SafeGetNumeric(ASC, AS, AS->GetMaxMajixAttribute())
    );*/
}

void ABaseCharacter::LogSecondaries_Client() const
{
	if (!IsLocallyControlled()) return;

	const UAbilitySystemComponent* ASC = AbilitySystemComponent;
	const UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	{
		UE_LOG(LogTemp, Warning, TEXT("[CLIENT] On Widget Init: "
		"Armor=%f ArmorPenetration=%f BlockChance=%f CriticalHitChance=%f CriticalHitDamage=%f CriticalHitResistance=%f "
		"Agility=%f Flexibility=%f Purity=%f Corruptibility=%f Intuition=%f Vision=%f Charm=%f "
		"HealthRegeneration=%f ShieldRegeneration=%f StaminaRegeneration=%f MajixRegeneration=%f "
		"MaxHealth=%f MaxShield=%f MaxStamina=%f MaxMajix=%f"),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetArmorAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetArmorPenetrationAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetBlockChanceAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitChanceAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitDamageAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitResistanceAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetAgilityAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetFlexibilityAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetPurityAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCorruptibilityAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetIntuitionAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetVisionAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCharmAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetHealthRegenerationAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetShieldRegenerationAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetStaminaRegenerationAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMajixRegenerationAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxHealthAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxShieldAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxStaminaAttribute()),
		SafeGetNumeric(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxMajixAttribute())
	);
	}
}
void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);;
}

void ABaseCharacter::AttackEnd()
{
	
}

void ABaseCharacter::DodgeEnd()
{
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MotionWarpingComponent) return;

	// First Warp Target – for translation
	FVector TranslationLocation = GetTranslationWarpTarget();
	FName TranslationWarpName = FName("TranslationTarget");

	// Second Warp Target – for rotation
	FVector RotationLocation = GetRotationWarpTarget();
	FName RotationWarpName = FName("RotationTarget");

	// Update both warp targets
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(TranslationWarpName, TranslationLocation);
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(RotationWarpName, RotationLocation);
}

UAnimMontage* ABaseCharacter::GetHitReactionMontage_Implementation()
{
	return HitReactionMontage;;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (!EquippedWeapon) return;

	if (EquippedMeleeWeapon == Cast<AMeleeWeapon>(EquippedWeapon))
	{
		EquippedMeleeWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ABaseCharacter::ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	CachedDamagedPawn = DamagedPawn;
	CachedDamage = Damage;
	CachedDamageType = DamageType;
	CachedInstigatorController = InstigatorController;
	CachedDamageCauser = DamageCauser;

}

void ABaseCharacter::PlayRandomMeleeAttackMontage()
{
	
}

void ABaseCharacter::PlayRandomMajixAttackMontage()
{
	
}

void ABaseCharacter::PlayAttackMontage()
{
	
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	CachedDamageAmount = DamageAmount;
	CachedDamageEvent = DamageEvent;
	CachedEventInstigator = EventInstigator;
	CachedDamageCauser = DamageCauser;	
		
	HandleDamage(CachedDamageAmount, CachedDamageEvent, CachedEventInstigator, CachedDamageCauser);
	return DamageAmount;
}

void ABaseCharacter::AddCharacterAbilities() const
{
	UHAFAbilitySystemComponent* ASComp = CastChecked<UHAFAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	ASComp->AddCharacterAbilities(StartupAbilities);
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	// We only cache a const set on BaseCharacter; return a mutable pointer for legacy call sites.
	return const_cast<UAttributeSet*>(AttributeSet.Get());
}
/*
void ABaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartCharacterDissolveTimeline(DynamicMatInst);
	}
	if (Weapon && IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* WeaponDynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, WeaponDynamicMatInst);

		StartWeaponDissolveTimeline(WeaponDynamicMatInst);
	}
} */

bool ABaseCharacter::IsAbilityInStartupAbilities(TSubclassOf<UGameplayAbility> AbilityToCheck) const
{
	if (!AbilityToCheck) return false;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass && AbilityClass->IsChildOf(AbilityToCheck))
		{
			return true; // ✅ Found match (AbilityToCheck or subclass)
		}
	}

	return false; // ❌ Not found
}

/*	// Small delay before playing actual section (prevents same-frame section override issues)
	FTimerHandle PlayHitReactHandle;
	FTimerDelegate PlayReactDel;
	PlayReactDel.BindLambda([this, Section]()
	{
		if (!GetMesh() || !GetMesh()->GetAnimInstance()) return;

		const float Result = GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage, 1.f);
		if (Result > 0.f)
		{
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(Section, HitReactMontage);
			UE_LOG(LogTemp, Warning, TEXT("🎯 Played section: %s"), *Section.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to play HitReact montage!"));
		}
	});
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(this))
	{
		if (!EnemyBase->EnemyCombat)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ EnemyCombat is NULL at time of damage!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("✅ EnemyCombat is valid at time of damage"));
		}
	}
	GetWorld()->GetTimerManager().SetTimer(PlayHitReactHandle, PlayReactDel, 0.05f, false); */

void ABaseCharacter::ResetHitReact()
{
	bCanReact = true;
}


// Optional: visual debug
/*
UKismetSystemLibrary::DrawDebugArrow(this, ActorLocation, ActorLocation + Forward * 100.f, 5.f, FColor::Red, 5.f);
UKismetSystemLibrary::DrawDebugArrow(this, ActorLocation, ActorLocation + ToHitNormalized * 100.f, 5.f, FColor::Green, 5.f);
*/





