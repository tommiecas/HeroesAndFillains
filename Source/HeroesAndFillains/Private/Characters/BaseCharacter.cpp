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
#include "HUD/EnemyHealthBarWidget.h"
#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"
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

#include "K2Node_CallFunction.h"
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
#include "AbilitySystem/HAFAttributeSet.h"
#include "PlayerController/FillainPlayerController.h"
#include "GameFramework/Controller.h"
#include "ModifiedMagnitudeCalculations/MMC_Agility.h"
#include "ModifiedMagnitudeCalculations/MMC_DexterityAgilityFlexibility.h"
#include "ModifiedMagnitudeCalculations/MMC_Intuition.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxHealth.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxMajix.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxShield.h"
#include "ModifiedMagnitudeCalculations/MMC_MaxStamina.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GetCapsuleComponent());

	GetMesh()->SetupAttachment(RootComponent);
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, MeleeAttackMontage)
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsA(AFillainCharacter::StaticClass()) && this->ActorHasTag(TEXT("FillainCharacter")))
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerCharacter);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pickupable, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Treasure, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Area, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	}
	else if (IsA(AEnemyBase::StaticClass()) && this->ActorHasTag(TEXT("Enemy")))
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	}
	else return;
}


void ABaseCharacter::InitializeAbilityActorInfo()
{
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// Early exit if actor is pending kill or invalid
	if (!IsValid(this)) return;

	// ✅ Apply cached damage BEFORE anything else
	TakeDamage(CachedDamageAmount, CachedDamageEvent, CachedEventInstigator, CachedDamageCauser);

	// Handle hit reaction
	if (AttributeComponent && AttributeComponent->IsCharacterAlive() && IsValid(HitReactMontage) && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		CharacterDies();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitSpecialEffects(ImpactPoint);
}

void ABaseCharacter::MeleeAttack()
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
		PlayDeathMontage();
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

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Determine direction of hit
	const FVector Forward = GetActorForwardVector();
	const FVector ActorLocation = GetActorLocation();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, ActorLocation.Z);
	const FVector ToHit = (ImpactLowered - ActorLocation).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section;

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	else
	{
		Section = FName("FromBack");
	}

	PlayHitReactMontage(Section);
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
	if (!DamagedActor || DamageInstigator)
	{
		const float CurrentCharm = GetHAFAttributeSet()->GetCharm();
		const float CharmPct = CurrentCharm / 100.f;
		if (CharmPct <= 0.f) return;
		
		const float Roll = FMath::FRandRange(0.f, 100.f);
		if (Roll <= CharmPct)
		{
			if (AEnemyBase* Enemy = Cast<AEnemyBase>(this))
			{
				Enemy->TriggerCharm(DamageInstigator);
			}
			else if (AFillainCharacter* Fillain = Cast<AFillainCharacter>(this))
			{
				Enemy->TriggerCharm(DamageInstigator);
			}
		}
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

int32 ABaseCharacter::PlayMeleeAttackMontage()
{
	
	return PlayRandomMontageSection(MeleeAttackMontage, MeleeAttackMontageSections);

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

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsCharacterAlive()
{
	return AttributeComponent && AttributeComponent->IsCharacterAlive();
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

void ABaseCharacter::ApplyStartupEffects()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	auto ApplyGE = [this](TSubclassOf<UGameplayEffect> GEClass)
	{
		if (!GEClass) return;
		FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
		Ctx.AddSourceObject(this);
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(GEClass, /*Level*/ 1.f, Ctx);
		if (Spec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	};
	ApplyGE(DefaultPrimaryAttributes);
	ApplyGE(DefaultSecondaryAttributes); // <-- where your Max* (e.g., MaxHealth) usually gets set
	ApplyGE(DefaultVitalAttributes);
	ApplyGE(DefaultInvisibleAttributes);
	if (HasAuthority() && AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SERVER] After ApplyStartupEffects: Armor=%f ArmorPenetration=%f BlockChance=%f CriticalHitChance=%f CriticalHitDamage=%f CriticalHitResistance=%f Agility=%f Flexibility=%f Purity=%f Corruptibility=%f Intuition=%f Vision=%f Charm=%f HealthRegeneration=%f ShieldRegeneration=%f StaminaRegeneration=%f MajixRegeneration=%f MaxHealth=%f MaxShield=%f MaxStamina=%f MaxMajix=%f"),
			HAFAttributeSet->GetArmor(),
			HAFAttributeSet->GetArmorPenetration(),
			HAFAttributeSet->GetBlockChance(),
			HAFAttributeSet->GetCriticalHitChance(),
			HAFAttributeSet->GetCriticalHitDamage(),
			HAFAttributeSet->GetCriticalHitResistance(),
			HAFAttributeSet->GetAgility(),
			HAFAttributeSet->GetFlexibility(),
			HAFAttributeSet->GetPurity(),
			HAFAttributeSet->GetCorruptibility(),
			HAFAttributeSet->GetIntuition(),
			HAFAttributeSet->GetVision(),
			HAFAttributeSet->GetCharm(),
			HAFAttributeSet->GetHealthRegeneration(),
			HAFAttributeSet->GetShieldRegeneration(),
			HAFAttributeSet->GetStaminaRegeneration(),
			HAFAttributeSet->GetMajixRegeneration(),
			HAFAttributeSet->GetMaxHealth(),
			HAFAttributeSet->GetMaxShield(),
			HAFAttributeSet->GetMaxStamina(),
			HAFAttributeSet->GetMaxMajix());
			
	}
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is NULL in InitializeDefaultAttributes"));
		return;
	}

	if (!HAFAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("HAFAttributeSet is NULL in InitializeDefaultAttributes (called too early?)"));
		return;
	}

	if (HasAuthority())
	{
		ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
		ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
		const auto MaxAttr = UHAFAttributeSet::GetMaxHealthAttribute();
		UE_LOG(LogTemp, Warning, TEXT("[ASC] MaxHealth=%f"),
		   AbilitySystemComponent->GetNumericAttribute(MaxAttr));

		if (const auto* HAF = AbilitySystemComponent->GetSet<UHAFAttributeSet>())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Set] MaxHealth=%f"), HAF->GetMaxHealth());
		}
		ApplyEffectToSelf((DefaultVitalAttributes), 1.f);
		ApplyEffectToSelf(DefaultInvisibleAttributes, 1.f);
		if (!AbilitySystemComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASC is NULL in InitializeDefaultAttributes"));
			return;
		}

		if (!HAFAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("HAFAttributeSet is NULL in InitializeDefaultAttributes (called too early?)"));
			return;
		}

		if (HasAuthority())
		{
			ApplyEffectToSelf(DefaultPrimaryAttributes, 1);
			ApplyEffectToSelf(DefaultSecondaryAttributes, 1);
		}

		// SAFE LOGGING (no direct FGameplayAttributeData::GetCurrentValue() derefs)
		UE_LOG(LogTemp, Warning, TEXT("[SERVER?=%d] After InitializeDefaultAttributes: "
			"Armor=%.3f ArmorPenetration=%.3f BlockChance=%.3f CriticalHitChance=%.3f CriticalHitDamage=%.3f CriticalHitResistance=%.3f "
			"Agility=%.3f Flexibility=%.3f Purity=%.3f Corruptibility=%.3f Intuition=%.3f Vision=%.3f Charm=%.3f "
			"HealthRegeneration=%.3f ShieldRegeneration=%.3f StaminaRegeneration=%.3f MajixRegeneration=%.3f "
			"MaxHealth=%.3f MaxShield=%.3f MaxStamina=%.3f MaxMajix=%.3f"),
			HasAuthority(),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetArmorAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetArmorPenetrationAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetBlockChanceAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitChanceAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitDamageAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCriticalHitResistanceAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetAgilityAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetFlexibilityAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetPurityAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCorruptibilityAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetIntuitionAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetVisionAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetCharmAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetHealthRegenerationAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetShieldRegenerationAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetStaminaRegenerationAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMajixRegenerationAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxHealthAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxShieldAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxStaminaAttribute()),
			SafeGet(AbilitySystemComponent, HAFAttributeSet, HAFAttributeSet->GetMaxMajixAttribute())
		);
		LogSecondaries_Server();
	}
}

void ABaseCharacter::LogSecondaries_Server() const
{
    const UAbilitySystemComponent* ASC = AbilitySystemComponent;
    const UHAFAttributeSet* AS = HAFAttributeSet;

    UE_LOG(LogTemp, Warning, TEXT("[SERVER] After InitializeDefaultAttributes: "
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
    );
}

void ABaseCharacter::LogSecondaries_Client() const
{
	if (!IsLocallyControlled()) return;

	if (AbilitySystemComponent && HAFAttributeSet)
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

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	CachedDamageAmount = DamageAmount;
	CachedDamageEvent = DamageEvent;
	CachedEventInstigator = EventInstigator;
	CachedDamageCauser = DamageCauser;	
		
	HandleDamage(CachedDamageAmount, CachedDamageEvent, CachedEventInstigator, CachedDamageCauser);
	return DamageAmount;
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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





