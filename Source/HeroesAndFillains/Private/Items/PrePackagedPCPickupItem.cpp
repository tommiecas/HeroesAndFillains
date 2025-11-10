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

void APrePackagedPCPickupItem::ApplyPickupEffect_Implementation(class AFillainCharacter* PlayerChar)
{
	if (!PlayerChar || !EffectToApply) return;

	UAbilitySystemComponent* TargetASC = PlayerChar->FindComponentByClass<UAbilitySystemComponent>();
	if (!TargetASC) return;

	// Get the source ASC (the pickup’s instigator or owner)
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());
	if (!SourceASC) SourceASC = TargetASC; // fallback

	// Optional: prevent self-application if you only want external effects
	// Allow same-ASC application for pickups (normal behavior when player collects).
	if (SourceASC == TargetASC)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Pickup %s applying effect to player ASC (expected)."), *GetName());
	}


	// Build the effect context from the *source* ASC
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	ContextHandle.AddInstigator(GetInstigator(), this);

	// Apply the GameplayEffect
	TargetASC->ApplyGameplayEffectToSelf(
		EffectToApply->GetDefaultObject<UGameplayEffect>(),
		1.f,
		ContextHandle
	);

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

