// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/StormWeapons.h"
#include "Characters/FillainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"  


AStormWeapons::AStormWeapons()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupWeaponBox();
	
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(MeleeWeaponMesh, TEXT("StartBoxTraceSocket"));
	

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(MeleeWeaponMesh, TEXT("EndBoxTraceSocket"));

	StormWeapon = this;
	
}

void AStormWeapons::SetupWeaponBox()
{
	if (!WeaponBox) return;

	// Attach to mesh or socket if needed
	WeaponBox->SetupAttachment(GetRootComponent()); // or a mesh/socket if appropriate

	// Make sure it's enabled for overlaps
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox); // Replace with your custom enum if needed

	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Make sure this matches your player's object type!
	WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);

	// Must be TRUE to receive OnComponentBeginOverlap
	WeaponBox->SetGenerateOverlapEvents(true);

	// Large enough to hit reliably (adjust as needed)
	WeaponBox->SetBoxExtent(FVector(10.f, 50.f, 50.f));

	// Optional: Debug visibility
	WeaponBox->bHiddenInGame = false;


	// Optional: Visual debug (for testing)
	// UE_LOG(LogTemp, Warning, TEXT("✅ WeaponBox setup complete — Extent: %s"), *WeaponBox->GetUnscaledBoxExtent().ToString());
}

void AStormWeapons::BeginPlay()
{
	Super::BeginPlay();

	/* Validate WeaponBox
	if (!WeaponBox)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ WeaponBox is null in %s"), *GetName());
		return;
	}*/

	// Initial Debug Color
	// FColor DebugColor = FColor::Red;

	// Check owner and tags
	AActor* WeaponOwner = GetOwner();
	if (IsValid(WeaponOwner))
	{
		if (WeaponOwner->Tags.Contains("Enemy"))
		{
			/* DebugColor = FColor::Blue;
			UE_LOG(LogTemp, Warning, TEXT("✅ WeaponOwner '%s' has 'Enemy' tag"), *WeaponOwner->GetName());
		}
		else if (WeaponOwner->Tags.Contains("Player"))
		{
			DebugColor = FColor::Green;
			UE_LOG(LogTemp, Warning, TEXT("✅ WeaponOwner '%s' has 'Player' tag"), *WeaponOwner->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ WeaponOwner '%s' has no recognized tag"), *WeaponOwner->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Weapon has no valid owner"));
	}*/

			// Collision Setup
			WeaponBox->SetBoxExtent(FVector(20.f, 100.f, 100.f));
			WeaponBox->SetHiddenInGame(false);
			WeaponBox->SetVisibility(true);
			WeaponBox->SetCollisionObjectType(ECC_EnemyWeaponBox);
			WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
			WeaponBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
			WeaponBox->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);

			// Log Collision Info
			UE_LOG(LogTemp, Warning, TEXT("📦 WeaponBox Setup"));
			UE_LOG(LogTemp, Warning, TEXT(" - ObjectType: %d"), WeaponBox->GetCollisionObjectType());
			UE_LOG(LogTemp, Warning, TEXT(" - Response to ECC_PlayerCharacter: %d"), WeaponBox->GetCollisionResponseToChannel(ECC_PlayerCharacter));
			UE_LOG(LogTemp, Warning, TEXT(" - Collision Profile: %s"), *WeaponBox->GetCollisionProfileName().ToString());

			// Attach Box to Mesh
			if (MeleeWeaponMesh)
			{
				WeaponBox->AttachToComponent(MeleeWeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("MeleeSocket"));
				WeaponBox->SetRelativeLocation(FVector::ZeroVector);

				// UE_LOG(LogTemp, Warning, TEXT("🔗 Attached WeaponBox to WeaponMesh at MeleeSocket"));
			}
			/* else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ WeaponMesh is null — cannot attach WeaponBox"));
			} */

			// Final Position Logs
			// UE_LOG(LogTemp, Warning, TEXT("📌 WeaponBox World Location: %s"), *WeaponBox->GetComponentLocation().ToString());
			// UE_LOG(LogTemp, Warning, TEXT("📌 WeaponBox Relative Location: %s"), *WeaponBox->GetRelativeLocation().ToString());

			// Overlap Bindings
			WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AStormWeapons::OnBoxOverlap);
			// UE_LOG(LogTemp, Warning, TEXT("🔗 Overlap delegate bound: %d"), WeaponBox->OnComponentBeginOverlap.IsBound());

			/* Debug Box (visual)
			DrawDebugBox(
				GetWorld(),
				WeaponBox->GetComponentLocation(),
				WeaponBox->GetScaledBoxExtent(),
				WeaponBox->GetComponentQuat(),
				DebugColor,
				false,
				5.0f,
				0,
				2.0f 
			); */


	
			/* Attachment verification
			if (WeaponBox->GetAttachParent() != WeaponMesh)
			{
				UE_LOG(LogTemp, Error, TEXT("❌ WeaponBox is not attached to WeaponMesh!"));
			}*/
		}
	}
}

void AStormWeapons::ClearDamagedActors()
{
	DamagedActors.Empty();
	// UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared"));
}

void AStormWeapons::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	PlayEquipSound();
	DeactivateEmbers();
}

void AStormWeapons::OnAttackHit(const FHitResult& HitResult)
{
	FVector ImpactPoint = HitResult.ImpactPoint;

	/*DrawDebugSphere(
		GetWorld(),
		ImpactPoint,
		20.f,
		12,
		FColor::Red,
		false,
		2.0f); */
}

void AStormWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsEquipped == true)
	{
		if (HoveringDecal) HoveringDecal->DestroyComponent();
		if (HoveringLight) HoveringLight->DestroyComponent();
		if (PickupWidgetComponent) PickupWidgetComponent->DestroyComponent();
		if (InfoWidgetComponent) InfoWidgetComponent->DestroyComponent();
		if (SphereCollision) SphereCollision->DestroyComponent();
	}
	
	/* DrawDebugBox(GetWorld(), WeaponBox->GetComponentLocation(), WeaponBox->GetScaledBoxExtent(), WeaponBox->GetComponentQuat(), FColor::Red, false, 2.f); */
}
void AStormWeapons::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Warning, TEXT("🔥 OnBoxOverlap triggered by: %s"), *GetNameSafe(OtherActor));

	// UE_LOG(LogTemp, Warning, TEXT("💥 OnBoxOverlap hit %s"), *GetNameSafe(OtherActor));

	// if (DamagedActors.Contains(OtherActor))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("⚠️ Already damaged actor this swing: %s"), *OtherActor->GetName());
	// 	return;
	// }

	// DamagedActors.Add(OtherActor);
	
	//if (ActorIsSameType(OtherActor)) return;

	/* UE_LOG(LogTemp, Warning, TEXT("📦 Overlapped Actor: %s | Tags: %s"), 
	   *GetNameSafe(OtherActor),
	   *FString::JoinBy(OtherActor->Tags, TEXT(", "), [](const FName& Name){ return Name.ToString(); })); */

	/* if (AFillainCharacter* HitChar = Cast<AFillainCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Error, TEXT("💥 Enemy weapon hit PLAYER: %s"), *HitChar->GetName());
	} */

	if (OtherActor && OtherActor != GetOwner())
	{
		// Already hit this actor during current swing?
		// if (DamagedActors.Contains(OtherActor)) return;

		// DamagedActors.Add(OtherActor);

		// UE_LOG(LogTemp, Error, TEXT("💥 Enemy weapon hit PLAYER: %s"), *OtherActor->GetName());
	
		FHitResult BoxHit;
		BoxTrace(BoxHit);

		AActor* HitActor = BoxHit.GetActor();
		if (!IsValid(HitActor) || ActorIsSameType(HitActor)) return;

		// ✅ Cache damage values before GetHit
		if (AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor))
		{
			if (Player->CombatComponent)
			{
				const FVector WeaponOrigin = GetActorLocation(); // or trace start
				const FVector HitLocation = BoxHit.ImpactPoint;

				FDamageEvent DamageEvent; // Use FPointDamageEvent or your custom subclass if needed

				Player->CombatComponent->ReceiveMeleeDamage(
					Damage,
					DamageEvent,
					Cast<AController>(GetOwner()->GetInstigatorController()), // Instigator
					this,       // DamageCauser (the weapon)
					WeaponOrigin,
					HitLocation
				);

				// UE_LOG(LogTemp, Warning, TEXT("✅ Called Combat->ReceiveMeleeDamage() on %s"), *Player->GetName());
			}
			/* else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ FillainCharacter CombatComponent was null!"));
			} */
		}
	}
}

void AStormWeapons::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	// UE_LOG(LogTemp, Warning, TEXT("NotifyActorBeginOverlap with: %s"), *OtherActor->GetName());
}

