// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PrePackagedPCPickupItem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "UI/ItemInfoWidgetBase.h"
#include "UI/PickupGearWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/AmmoPickup.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Interfaces/PickupInterface.h"

APrePackagedPCPickupItem::APrePackagedPCPickupItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
	
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PP_ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetGenerateOverlapEvents(false);
	ItemMesh->SetUsingAbsoluteLocation(false);
	ItemMesh->SetUsingAbsoluteRotation(false);
	ItemMesh->SetUsingAbsoluteScale(false);
	ItemMesh->SetRelativeTransform(FTransform::Identity);
	
	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	ItemEffect->SetupAttachment(RootComponent, TEXT("EffectSocket"));   // <-- use SetupAttachment
	ItemEffect->SetRelativeTransform(FTransform::Identity);
	ItemEffect->bAutoActivate = true;
	ItemEffect->SetUsingAbsoluteLocation(false);
	ItemEffect->SetUsingAbsoluteRotation(false);
	ItemEffect->SetUsingAbsoluteScale(false);
	
	check(AreaSphere);
	AreaSphere->SetSphereRadius(125.f, true);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionObjectType(ECC_Pickupable); // Or your custom PCWeaponBox
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	AreaSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));
	AreaSphere->SetHiddenInGame(true);
	AreaSphere->ShapeColor = FColor::Green;
	AreaSphere->SetCanEverAffectNavigation(false);

	AreaShape  = AreaSphere; // base will wire HiddenTreasure to this
}

void APrePackagedPCPickupItem::BeginPlay()
{
	Super::BeginPlay();

	

	/* if (!AreaSphere)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ AreaSphere is NULL in actor: %s, class: %s"),
			*GetName(),
			*GetClass()->GetName());
		return;
	}*/

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &APrePackagedPCPickupItem::OnOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &APrePackagedPCPickupItem::OnEndOverlap);
}

void APrePackagedPCPickupItem::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void APrePackagedPCPickupItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void APrePackagedPCPickupItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetGenerateOverlapEvents(false);
	}
}

void APrePackagedPCPickupItem::EnableCustomDepth(bool bEnable)
{
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bEnable);
	}
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bEnable);
	}
}

void APrePackagedPCPickupItem::ApplyPickupEffect_Implementation(AFillainCharacter* PlayerChar)
{
	if (!PlayerChar || !EffectToApply)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup %s: Missing PlayerChar or EffectToApply!"), *GetName());
		return;
	}

	// ✅ Get target ASC (the player's)
	UAbilitySystemComponent* TargetASC = PlayerChar->FindComponentByClass<UAbilitySystemComponent>();
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup %s: Player %s has no ASC!"), *GetName(), *GetNameSafe(PlayerChar));
		return;
	}

	// ✅ Try to get a source ASC (the pickup’s instigator)
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());
	if (!SourceASC)
	{
		// If pickup has no ASC (typical), use target ASC as fallback
		SourceASC = TargetASC;
	}

	// ✅ Build a valid effect context from whichever ASC we have
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	ContextHandle.AddInstigator(GetInstigator(), this);

	// ✅ Create the outgoing spec properly (never use GetDefaultObject)
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(EffectToApply, 1.f, ContextHandle);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup %s: Failed to create spec for %s"), *GetName(), *GetNameSafe(EffectToApply.Get()));
		return;
	}

	// ✅ Apply it safely
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	UE_LOG(LogTemp, Log, TEXT("Pickup %s applied %s to %s"),
		*GetNameSafe(this),
		*GetNameSafe(EffectToApply.Get()),
		*GetNameSafe(PlayerChar));

	// ✅ Optional: trigger pickup FX
	if (ItemEffect)
	{
		ItemEffect->Activate(true);
	}
}


void APrePackagedPCPickupItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	
		// Set OverlappingItem for non-weapon item types
		PickupInterface->SetOverlappingItem(this);
}

void APrePackagedPCPickupItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (!PickupInterface) return;

	// ⚠️ Don't clear if this item is already equipped
	if (ItemState == EItemState::EIS_Equipped) return;

	SetOwner(OtherActor);
	if (GetOwner()) return;

	PickupInterface->SetOverlappingItem(nullptr);
}

void APrePackagedPCPickupItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation()
		);
	}
}

void APrePackagedPCPickupItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}

void APrePackagedPCPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
	
}

void APrePackagedPCPickupItem::ShowPickupAndInfoWidgets(bool bShow)
{
	if (PickupGearWidgetComponent) PickupGearWidgetComponent->SetVisibility(bShow);
	if (ItemInfoWidgetComponent) ItemInfoWidgetComponent->SetVisibility(bShow);
}

