// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
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

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// Setup mesh component
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		MeshComponent->SetupAttachment(GetCapsuleComponent());
		MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetGenerateOverlapEvents(true);
	}

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	// Create the WidgetComponent
	NewHealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	NewHealthBarWidgetComponent->SetupAttachment(GetCapsuleComponent());
	NewHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NewHealthBarWidgetComponent->SetDrawSize(FVector2D(300.f, 25.f));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBase::PlayHitReactMontage(const FName& SectionName)
{
	if (!IsValid(HitReactMontage))
	{
		return;
	}
        
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemyBase::PlayDeathMontage()
{
	if (!IsValid(DeathMontage))
	{
		return;
	}
	
	UAnimInstance* Instance = GetMesh()->GetAnimInstance(); 
	if (!Instance)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ AnimInstance is NULL"));
		return;
	}

	if (!DeathMontage)
	{
		// UE_LOG(LogTemp, Error, TEXT("❌ AttackMontage is NULL"));
		return;
	}

	// At this point, everything is good
	UE_LOG(LogTemp, Warning, TEXT("✅ Playing Death Montage"));
}
	

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistanceToTarget > CombatRadius)
		{
			CombatTarget = nullptr;
			if (NewHealthBarWidgetComponent)
			{
				NewHealthBarWidgetComponent->SetVisibility((false));
			}
		}
	}

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::DirectionalHitReact(const FVector& ImpactPoint)
{
	if (!IsValid(this))
	{
		return;
	}

	const FVector Forward = GetActorForwardVector();
	if (Forward.IsZero())
	{
		return;
	}

	const FVector ActorLocation = GetActorLocation();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, ActorLocation.Z);
	const FVector ToHit = (ImpactLowered - ActorLocation);
	const FVector ToHitNormalized = ToHit.GetSafeNormal();
    
	// Ensure we're not dealing with a zero-length vector
	if (ToHit.IsNearlyZero())
	{
		PlayHitReactMontage(FName("FromFront"));
		return;
	}

	const double CosTheta = FVector::DotProduct(Forward, ToHitNormalized);
    
	/* // Protect against invalid input to Acos
	if (CosTheta < -1.0 || CosTheta > 1.0)
	{
		PlayHitReactMontage(FName("FromFront"));
		return;
	} */

	double Theta = FMath::Acos(CosTheta);
    Theta = FMath::RadiansToDegrees(Theta);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHitNormalized);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack");
	
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

	PlayHitReactMontage(Section);


	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);

	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComponent && NewHealthBarWidgetComponent)
	{
		AttributeComponent->EnemiesReceiveMeleeDamage(DamageAmount);
		NewHealthBarWidgetComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}
	CombatTarget = EventInstigator->GetPawn();
	return DamageAmount;
}

void AEnemyBase::GetHit_Implementation(const FVector& ImpactPoint)
{
	// Early exit if actor is pending kill or invalid
	if (!IsValid(this))
	{
		return;
	}

	// Cache mesh component to avoid multiple GetMesh() calls
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (!IsValid(MeshComponent))
	{
		return;
	}

	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((true));
	}
	
	// Handle hit reaction
	if (IsValid(HitReactMontage) && AttributeComponent && AttributeComponent->IsCharacterAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else if (IsValid(DeathMontage) && AttributeComponent && AttributeComponent->IsCharacterAlive() == false)
	{
		EnemyDies();
	}

	// Play sound if available
	if (IsValid(HitSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HitSound,
			ImpactPoint
		);
	}

	// Spawn hit effect if available
	if (IsValid(HitReactSystem))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitReactSystem,
			ImpactPoint,
			GetActorRotation()
		);
	}
}

void AEnemyBase::EnemyDies()
{
	PlayDeathMontage();
	if (NewHealthBarWidgetComponent)
	{
		NewHealthBarWidgetComponent->SetVisibility((false));
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}



