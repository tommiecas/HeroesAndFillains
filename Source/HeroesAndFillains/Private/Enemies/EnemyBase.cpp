// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeroesAndFillains/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"  

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	if (!Root)
	{
		Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
		if (Root)
		{
			SetRootComponent(Root);
		}
	}

	// Setup mesh component
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		MeshComponent->SetupAttachment(Root);
		MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		MeshComponent->SetGenerateOverlapEvents(true);
	}

	// Setup capsule component
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (CapsuleComp)
	{
		CapsuleComp->SetupAttachment(Root);
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	}

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
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
    
	// Ensure we're not dealing with a zero-length vector
	if (ToHit.IsNearlyZero())
	{
		PlayHitReactMontage(FName("FromFront"));
		return;
	}

	const FVector ToHitNormalized = ToHit.GetSafeNormal();
	const double CosTheta = FVector::DotProduct(Forward, ToHitNormalized);
    
	// Protect against invalid input to Acos
	if (CosTheta < -1.0 || CosTheta > 1.0)
	{
		PlayHitReactMontage(FName("FromFront"));
		return;
	}

	double Theta = FMath::RadiansToDegrees(FMath::Acos(CosTheta));
    
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHitNormalized);
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


	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);

	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
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

	// Handle hit reaction
	if (IsValid(HitReactMontage))
	{
		DirectionalHitReact(ImpactPoint);
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



