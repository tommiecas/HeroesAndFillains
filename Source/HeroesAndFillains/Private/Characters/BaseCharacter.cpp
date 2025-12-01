// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "HAFGameplayTags.h"
#include "GameplayEffectTypes.h"

#include "Weapons/WeaponBase.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/Abilities/HAFGameplayAbility.h"

#include "PlayerController/FillainPlayerController.h"
#include "GameMode/HaFGameMode.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/SpectralBase.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "HAFComponents/AttributeComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// TODO: Remove AttributeComponent after migrating all code to GAS
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

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABaseCharacter::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
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
	
	// Note: Player-specific initialization (like AttackTags setup) should be done in FillainCharacter::BeginPlay()
	// to avoid circular dependency between BaseCharacter and FillainCharacter
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

	Spec->SetSetByCallerMagnitude(TAG_VitalEffects_StaminaCost, -DodgeCost);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
}

void ABaseCharacter::Die_Implementation()
{
	if (bIsCharacterDead)  // or bIsDead—pick one shared flag!
	{
		return;
	}
	bIsCharacterDead = true;
	
	if (ActorHasTag("Enemy"))
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(this);
		Enemy->EnemyState = EEnemyState::EES_Dead;
		Enemy->ClearAttackTimer();
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
	
 
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(
			FDetachmentTransformRules(EDetachmentRule::KeepWorld, true)
		);
	}

	CharacterDies();        // Server-side death logic
	MulticastHandleDeath(); // Cosmetic death, effects, ragdoll, etc.
}


void ABaseCharacter::InitializeDefaultTags()
{
	
}

void ABaseCharacter::SafeInitializeAttributes()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ❌ Cannot SafeInitializeAttributes — ASC is missing."), *GetName());
		return;
	}

	// If the ASC is not ready yet, schedule again next tick
	if (!ASC->AbilityActorInfo.IsValid())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[%s] ⚠️ AbilityActorInfo not ready yet — deferring SafeInitializeAttributes."), *GetName());
		GetWorldTimerManager().SetTimerForNextTick(this, &ABaseCharacter::SafeInitializeAttributes);
		return;
	}

	// Only initialize once on the server
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] ✅ SafeInitializeAttributes(): proceeding with InitializeDefaultAttributes."), *GetName());
		InitializeDefaultAttributes();
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("[%s] Skipping SafeInitializeAttributes (client)."), *GetName());
	}
}

void ABaseCharacter::MulticastHandleDeath_Implementation()
{
	// Mark as dead FIRST so IsDead() returns true immediately
	bIsCharacterDead = true;
	
	if (EquippedWeapon && EquippedWeapon->GetWeaponMesh())
	{
		EquippedWeapon->GetWeaponMesh()->SetSimulatePhysics(true);
		EquippedWeapon->GetWeaponMesh()->SetEnableGravity(true);
		EquippedWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	// DON'T enable physics on mesh - let the death montage play instead
	// The montage will hold the dead pose until dissolve completes
	if (GetMesh())
	{
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	Dissolve();

	if (ASpectralBase* SpectralSoldier = Cast<ASpectralBase>(this))
	{
		SpectralSoldier->SpectralAssaultRifle->DestroyComponent();
	}
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
	}
	
	// Note: FillainCharacter-specific death handling moved to FillainCharacter::Die() override
	// This keeps BaseCharacter independent of FillainCharacter to avoid circular dependencies
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

// Legacy damage system stubs - kept for child class compatibility
// TODO: Remove after cleaning up EnemyBase and FillainCharacter
float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// Empty stub - child classes may override
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseCharacter::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// Empty stub - child classes may override
}

void ABaseCharacter::ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	// Empty stub - child classes may override
}

EEnemyType ABaseCharacter::GetEnemyType_Implementation()
{
	return EnemyType;
}

void ABaseCharacter::PlayRandomMeleeAttackMontage()
{
	// Empty stub - child classes may override
}

void ABaseCharacter::PlayRandomMajixAttackMontage()
{
	// Empty stub - child classes may override
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

TArray<FVector> ABaseCharacter::GetCombatSocketLocations_Implementation(const FGameplayTag& SocketTag)
{
    TArray<FVector> Locations;
    const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();
    
    // Single socket attacks
    if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
    {
        if (IsValid(EquippedWeapon) && EquippedWeapon->GetWeaponMesh())
        {
            Locations.Add(EquippedWeapon->GetWeaponMesh()->GetSocketLocation(WeaponTipSocketName));
        }
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
    {
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftFoot))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightFoot))
    {
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
    // Multi-socket attacks
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_BothHands))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_BothFeet))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
    else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_HandsAndFeet))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
	else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_WeaponAndFeet))
	{
		if (IsValid(EquippedWeapon) && EquippedWeapon->GetWeaponMesh()) Locations.Add(EquippedWeapon->GetWeaponMesh()->GetSocketLocation(WeaponTipSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
	}
	else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_WeaponAndHands))
	{
		if (IsValid(EquippedWeapon) && EquippedWeapon->GetWeaponMesh()) Locations.Add(EquippedWeapon->GetWeaponMesh()->GetSocketLocation(WeaponTipSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
	}
	else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_WeaponAndHandsAndFeet))
	{
		if (IsValid(EquippedWeapon) && EquippedWeapon->GetWeaponMesh()) Locations.Add(EquippedWeapon->GetWeaponMesh()->GetSocketLocation(WeaponTipSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
		Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
	}
	else if (SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Teeth))
	{
		Locations.Add(GetMesh()->GetSocketLocation(TeethSocketName));
	}
    
    return Locations;
}

FTaggedMontage ABaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag) return TaggedMontage;
	}
	return FTaggedMontage();
}


bool ABaseCharacter::IsDead_Implementation() const
{
	return bIsCharacterDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Returning %d attack montages."), *GetName(), AttackMontages.Num());
	for (auto& M : AttackMontages)
	{
		UE_LOG(LogTemp, Warning, TEXT(" - %s (%s)"), *GetNameSafe(M.Montage), *M.MontageTag.ToString());
	}
	return AttackMontages;}

UNiagaraSystem* ABaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

bool ABaseCharacter::CanAttack()
{
	if (AEnemyBase* PossibleEnemy = Cast<AEnemyBase>(this))
	{
		if (bIsCharacterDead || PossibleEnemy->EnemyState == EEnemyState::EES_Dead)
		{
			return false;
		}
	}
	return true;
}

bool ABaseCharacter::IsCharacterAlive() const
{
	return GetCurrentHealth() > KINDA_SMALL_NUMBER;
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level)
{
	if (!IsValid(GetAbilitySystemComponent()) || !GameplayEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyEffectToSelf failed: Missing ASC or invalid GameplayEffectClass on %s"), *GetName());
		return;
	}

	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent();
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyEffectToSelf failed: Missing ASC on %s"), *GetName());
		return;
	}

	// Build effect context safely using this ASC
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	if (SpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

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
	// UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());

	ApplyGE(DefaultVitalAttributes);
	// UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());

	ApplyGE(DefaultInvisibleAttributes);

	/* UE_LOG(LogTemp, Warning, TEXT("Attributes applied: %f/%f"), HAFAS->GetHealth(), HAFAS->GetMaxHealth());
	
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
    }*/
}

void ABaseCharacter::InitializeDefaultAttributes()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// --- Safety checks ---
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ❌ Missing AbilitySystemComponent in InitializeDefaultAttributes!"), *GetName());
		return;
	}

	if (!ASC->AbilityActorInfo.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ⚠️ ASC AbilityActorInfo not yet valid — deferring InitializeDefaultAttributes."), *GetName());
		GetWorldTimerManager().SetTimerForNextTick(this, &ABaseCharacter::InitializeDefaultAttributes);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeDefaultAttributes() | ASC: %s | Avatar: %s | Controller: %s"),
		*GetName(),
		*GetNameSafe(ASC),
		*GetNameSafe(ASC->GetAvatarActor()),
		*GetNameSafe(ASC->AbilityActorInfo->PlayerController.Get()));

	// --- Apply only on authority ---
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[%s] Skipping attribute initialization (client)."), *GetName());
		return;
	}

	auto LogGE = [&](const TCHAR* Label, TSubclassOf<UGameplayEffect> GE)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Applying %s: %s"), *GetNameSafe(this), Label, *GetNameSafe(GE));
	};

	// --- Apply each valid GameplayEffect safely ---
	if (DefaultPrimaryAttributes)
	{
		LogGE(TEXT("DefaultPrimaryAttributes"), DefaultPrimaryAttributes);
		ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	}

	if (DefaultSecondaryAttributes)
	{
		LogGE(TEXT("DefaultSecondaryAttributes"), DefaultSecondaryAttributes);
		ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	}

	if (DefaultResistanceAttributes)
	{
		LogGE(TEXT("DefaultResistanceAttributes"), DefaultResistanceAttributes);
		ApplyEffectToSelf(DefaultResistanceAttributes, 1.f);
	}

	if (DefaultVitalAttributes)
	{
		LogGE(TEXT("DefaultVitalAttributes"), DefaultVitalAttributes);
		ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	}

	if (DefaultInvisibleAttributes)
	{
		LogGE(TEXT("DefaultInvisibleAttributes"), DefaultInvisibleAttributes);
		ApplyEffectToSelf(DefaultInvisibleAttributes, 1.f);
	}

	// Optional: server-side diagnostic
	LogSecondaries_Server();

	UE_LOG(LogTemp, Log, TEXT("[%s] ✅ Finished applying BaseCharacter default attributes safely."), *GetNameSafe(this));
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
	if (bIsCharacterDead)
	{
		return;
	}
	
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

void ABaseCharacter::AddCharacterAbilities()
{
	UHAFAbilitySystemComponent* ASCComp = CastChecked<UHAFAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	ASCComp->AddCharacterAbilities(StartupAbilities);
	ASCComp->AddCharacterPassiveAbilities(StartupPassiveAbilities);
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

void ABaseCharacter::Dissolve()
{
	UE_LOG(LogTemp, Warning, TEXT("💨 %s Dissolve() called"), *GetName());
	
	// Character dissolve
	if (IsValid(CharacterDissolveMaterialInstanceZero))
	{
		UMaterialInstanceDynamic* CharacterDynamicMatInstZero = UMaterialInstanceDynamic::Create(CharacterDissolveMaterialInstanceZero, this);
		GetMesh()->SetMaterial(0, CharacterDynamicMatInstZero);

		StartCharacterDissolveTimelineZero(CharacterDynamicMatInstZero);
		UE_LOG(LogTemp, Warning, TEXT("   ✅ Character dissolve material applied"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("   ❌ CharacterDissolveMaterialInstanceZero not assigned!"));
	}
	
	// Weapon dissolve
	if (EquippedWeapon && EquippedWeapon->GetWeaponMesh() && IsValid(WeaponDissolveMaterialInstanceZero))
	{
		UMaterialInstanceDynamic* WeaponDynamicMatInstZero = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstanceZero, this);
		EquippedWeapon->GetWeaponMesh()->SetMaterial(0, WeaponDynamicMatInstZero);

		StartWeaponDissolveTimelineZero(WeaponDynamicMatInstZero);
		UE_LOG(LogTemp, Warning, TEXT("   ✅ Weapon dissolve material applied"));
	}
}

bool ABaseCharacter::IsAbilityInStartupAbilities(TSubclassOf<UGameplayAbility> AbilityToCheck) const
{
	if (!AbilityToCheck) return false;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass && AbilityClass->IsChildOf(AbilityToCheck))
		{
			return true;
		}
	}

	return false;
}





