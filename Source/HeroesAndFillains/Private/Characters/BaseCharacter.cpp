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
#include "HUD/HealthBarWidget.h"
#include "HUD/HealthBarWidgetComponent.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
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
#include "Enemies/EnemyBase.h"
#include "GameStates/HAFGameState.h"  
#include "PlayerStart/TeamPlayerStart.h"
#include "GameFramework/Actor.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "Characters/FillainFinalAnimInstance.h"
#include "Animation/AnimInstance.h"



ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	// Create root component
	SetRootComponent(GetCapsuleComponent());

	AActor* WeaponOwner = GetOwner();
	if (IsValid(WeaponOwner) && WeaponOwner->ActorHasTag("FillainCharacter"))
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerCharacter);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponBox, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	}
	else if (IsValid(WeaponOwner) && WeaponOwner->ActorHasTag("Enemy"))
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	}
	else
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldStatic);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (!EquippedWeapon) return;

	if (EquippedMeleeWeapon == Cast<AMeleeWeapon>(EquippedWeapon))
	{
		EquippedMeleeWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
}

int32 ABaseCharacter::PlayMeleeAttackMontage()
{
	// UE_LOG(LogTemp, Warning, TEXT("🎬 PlayMeleeAttackMontage() triggered"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); 
	if (!AnimInstance)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ AnimInstance is NULL"));

		if (MeleeAttackMontageSections.Num() <= 0) return -1;
		int32 MaxSectionIndex = MeleeAttackMontageSections.Num() - 1;
		const int32 Selection = MaxSectionIndex;
		return Selection;	
	}

	if (!MeleeAttackMontage)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ MeleeAttackMontage is NULL"));
		if (MeleeAttackMontageSections.Num() <= 0) return -1;
		int32 MaxSectionIndex = MeleeAttackMontageSections.Num() - 1;
		const int32 Selection = MaxSectionIndex;
		return Selection;
	}

	if (!Combat)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Combat null"));
		if (MeleeAttackMontageSections.Num() <= 0) return -1;
		int32 MaxSectionIndex = MeleeAttackMontageSections.Num() - 1;
		const int32 Selection = MaxSectionIndex;
		return Selection;
	}

	// At this point, everything is good
	UE_LOG(LogTemp, Warning, TEXT("✅ Playing Melee Attack Montage"));
	UE_LOG(LogTemp, Warning, TEXT("🧪 Calling PlayRandomMontageSection()..."));
	return PlayRandomMontageSection(MeleeAttackMontage, MeleeAttackMontageSections);

}

void ABaseCharacter::MeleeAttack()
{
}

bool ABaseCharacter::CanAttack()
{
	return true;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::CharacterDies()
{
}

int32 ABaseCharacter::PlayNothingByReturningDefaultMax(TArray<FName> Sections)
{
	if (Sections.Num() <= 0) return -1;
	const int32 MaxSectionIndex = Sections.Num() - 1;
	const int32 Selection = MaxSectionIndex;
	return Selection;
}

void ABaseCharacter::ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (AFillainCharacter* Fillain = Cast<AFillainCharacter>(DamagedPawn))
	{
		Fillain->HAFGameMode = Fillain->HAFGameMode == nullptr ? GetWorld()->GetAuthGameMode<AHAFGameMode>() : Fillain->HAFGameMode;
		if (Fillain->bIsEliminated || !IsValid(Fillain->HAFGameMode) || Damage <= 0.f) return;

		float DamageToHealth = Fillain->CalculateDamage(DamagedPawn, Damage, InstigatorController);
		
		Fillain->CalculateShieldDamage(Damage, DamageToHealth);
		Fillain->AttributeComponent->CharactersReceiveMeleeDamage(Damage);
		Fillain->UpdateHUDHealth();
		Fillain->UpdateHUDShield();

		// ✅ Use your existing directional logic
		Fillain->DirectionalHitReact(DamageCauser ? DamageCauser->GetActorLocation() : GetActorLocation());

		// CacheDamageParameters(DamagedPawn, Damage, DamageType, InstigatorController, DamageCauser);

		if (Fillain->Health == 0.f)
		{
			Fillain->DetermineRolesOnPlayerDeath(DamagedPawn, InstigatorController);
		}

		Fillain->ResetCachedDamageParameters();
	}
	else if (AEnemyBase* BadGuy = Cast<AEnemyBase>(DamagedPawn))
	{
		CalculateDamage(DamagedPawn, Damage, InstigatorController);
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	if (!HitReactMontage || !GetMesh()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	FString DirectionStr = SectionName.ToString().Replace(TEXT("From"), TEXT("")); // e.g., "FromFront" -> "Front"
	const FName ResetSection = FName(*FString::Printf(TEXT("Reset%s"), *DirectionStr));

	if (HitReactMontage->IsValidSectionName(ResetSection))
	{
		UE_LOG(LogTemp, Warning, TEXT("🎯 Jumping to Reset Section: %s"), *ResetSection.ToString());

		// Force restart logic
		AnimInstance->Montage_Play(HitReactMontage, 1.f);
		AnimInstance->Montage_JumpToSection(ResetSection);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("🎯 No reset section. Playing normally."));

		AnimInstance->Montage_Stop(0.1f, HitReactMontage);
		AnimInstance->Montage_Play(HitReactMontage, 1.f);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

int32 ABaseCharacter::PlayDeathMontage()
{
	if (!IsValid(DeathMontage))
	{
		PlayNothingByReturningDefaultMax(DeathMontageSections);
	}
	
	UAnimInstance* Instance = GetMesh()->GetAnimInstance(); 
	if (!Instance)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ AnimInstance is NULL"));
		PlayNothingByReturningDefaultMax(DeathMontageSections);
	}

	if (!DeathMontage)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ AttackMontage is NULL"));
		PlayNothingByReturningDefaultMax(DeathMontageSections);
	}

	// At this point, everything is good
	UE_LOG(LogTemp, Warning, TEXT("✅ Playing Death Montage"));

	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	// Early exit if actor is pending kill or invalid
	if (!IsValid(this)) return;

	// ✅ Apply cached damage BEFORE anything else
	TakeDamage(CachedDamageAmount, CachedDamageEvent, CachedEventInstigator, CachedDamageCauser);

	// Handle hit reaction
	if (AttributeComponent && AttributeComponent->IsCharacterAlive() && IsValid(HitReactMontage))
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		CharacterDies();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitSpecialEffects(ImpactPoint);
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("🎯 DirectionalHitReact triggered for %s"), *GetName());
	if (!IsValid(this) || !GetMesh() || !GetMesh()->GetAnimInstance() || !HitReactMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ DirectionalHitReact missing critical data."));
		return;
	}

	// Determine direction of hit
	const FVector Forward = GetActorForwardVector();
	const FVector ActorLocation = GetActorLocation();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, ActorLocation.Z);
	const FVector ToHit = (ImpactLowered - ActorLocation).GetSafeNormal();

	FName SectionToPlay = "FromFront"; // Default
	if (!ToHit.IsNearlyZero())
	{
		double Theta = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(Forward, ToHit), -1.0, 1.0))
		);
		if (FVector::CrossProduct(Forward, ToHit).Z < 0.f) Theta *= -1.f;

		if (Theta >= -45.f && Theta < 45.f)
			SectionToPlay = "FromFront";
		else if (Theta >= -135.f && Theta < -45.f)
			SectionToPlay = "FromLeft";
		else if (Theta >= 45.f && Theta < 135.f)
			SectionToPlay = "FromRight";
		else
			SectionToPlay = "FromBack";
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HitReactMontage) return;

	// Skip if not allowed
	if (!bCanReact)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Skipping HitReact: bCanReact is false"));
		return;
	}

	bCanReact = false;
	GetWorld()->GetTimerManager().SetTimer(HitReactTimer, this, &ABaseCharacter::ResetHitReact, 0.25f, false);

	// Force reset via Reset section BEFORE jumping to section
	const FString DirectionName = SectionToPlay.ToString().Replace(TEXT("From"), TEXT(""));
	const FName ResetSection = FName(*FString::Printf(TEXT("Reset%s"), *DirectionName));
	if (HitReactMontage->IsValidSectionName(ResetSection))
	{
		UE_LOG(LogTemp, Warning, TEXT("🔁 Forcing reset by jumping to: %s"), *ResetSection.ToString());

		// Step 1: Play the montage if needed
		AnimInstance->Montage_Play(HitReactMontage, 1.f);

		// Step 2: Set next section from Reset_Section back to actual HitReact section
		AnimInstance->Montage_SetNextSection(ResetSection, SectionToPlay, HitReactMontage);
		UE_LOG(LogTemp, Warning, TEXT("➡️ Set next section after %s to %s"), *ResetSection.ToString(), *SectionToPlay.ToString());

		// Step 3: Jump to the Reset section now
		AnimInstance->Montage_JumpToSection(ResetSection, HitReactMontage);
	}

	// Small delay before playing actual section (prevents same-frame section override issues)
	FTimerHandle PlayHitReactHandle;
	FTimerDelegate PlayReactDel;
	PlayReactDel.BindLambda([this, SectionToPlay]()
	{
		if (!GetMesh() || !GetMesh()->GetAnimInstance()) return;

		const float Result = GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage, 1.f);
		if (Result > 0.f)
		{
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionToPlay, HitReactMontage);
			UE_LOG(LogTemp, Warning, TEXT("🎯 Played section: %s"), *SectionToPlay.ToString());
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
	GetWorld()->GetTimerManager().SetTimer(PlayHitReactHandle, PlayReactDel, 0.05f, false);
}

void ABaseCharacter::ResetHitReact()
{
	bCanReact = true;
}
// Optional: visual debug
/*
UKismetSystemLibrary::DrawDebugArrow(this, ActorLocation, ActorLocation + Forward * 100.f, 5.f, FColor::Red, 5.f);
UKismetSystemLibrary::DrawDebugArrow(this, ActorLocation, ActorLocation + ToHitNormalized * 100.f, 5.f, FColor::Green, 5.f);
*/

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("⚔️ Calling TakeDamage — CachedDamageAmount: %f"), CachedDamageAmount);
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	

	return DamageAmount;
}

float ABaseCharacter::CalculateDamage(AActor* DamagedPawn, float DamageAmount,
	AController* InstigatorController)
{
	if (AttributeComponent)
	{
		float DamageToHealth = AttributeComponent->CharactersReceiveMeleeDamage(DamageAmount);

		if (AttributeComponent && DamagedPawn->IsA(AEnemyBase::StaticClass()) && NewHealthBarWidgetComponent)
		{
			NewHealthBarWidgetComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
            
			if (AttributeComponent->IsCharacterAlive() == false)
			{
				CharacterDies();
				return 0.f;
			}
			return DamageToHealth;
		}
		else if (AttributeComponent && DamagedPawn->IsA(AFillainCharacter::StaticClass()))
		{
			AFillainCharacter* Fillain = Cast<AFillainCharacter>(DamagedPawn);
			Fillain->GetFillainPlayerController()->SetHUDHealth(AttributeComponent->Health, AttributeComponent->MaxHealth);
			Fillain->AttributeComponent->GetHealthPercent();
			Fillain->UpdateHUDHealth();
			if (AttributeComponent->IsCharacterAlive() == false)
			{
				Fillain->DetermineRolesOnPlayerDeath(DamagedPawn, InstigatorController);
				return 0.f;
			}
			return DamageToHealth;
		}
        
		return DamageAmount;
	}
	return DamageAmount; // Added default return when AttributeComponent is null
}

bool ABaseCharacter::IsCharacterAlive()
{
	return AttributeComponent->IsCharacterAlive();
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

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (!Montage || SectionName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Invalid Montage or SectionName"));
		return;
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ AnimInstance is nullptr"));
		return;
	}

	const int32 SectionIndex = Montage->GetSectionIndex(SectionName);
	if (SectionIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Section '%s' not found in montage '%s'"), *SectionName.ToString(), *Montage->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("▶️ Playing montage '%s', section '%s'"), *Montage->GetName(), *SectionName.ToString());

	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(SectionName, Montage);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> SectionNames)
{
	if (!Montage)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Montage is nullptr!"));
		return -1;
	}

	if (SectionNames.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No section names provided!"));
		return -1;
	}

	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	const FName& SelectedSection = SectionNames[Selection];

	int32 SectionIndex = Montage->GetSectionIndex(SelectedSection);
	if (SectionIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Section '%s' not found in montage '%s'"), *SelectedSection.ToString(), *Montage->GetName());
		return -1;
	}

	UE_LOG(LogTemp, Warning, TEXT("✅ Playing section '%s' from montage '%s'"), *SelectedSection.ToString(), *Montage->GetName());
	UE_LOG(LogTemp, Warning, TEXT("📢 SectionNames count = %d"), SectionNames.Num());
	PlayMontageSection(Montage, SelectedSection);
	return Selection;
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}