// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/FillainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "HUD/ItemInfoWidgetBase.h"
#include "HUD/PickupGearWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/AmmoPickup.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Interfaces/PickupInterface.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetGenerateOverlapEvents(false);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetSphereRadius(200.f);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionObjectType(ECC_WorldDynamic); // Or your custom PCWeaponBox
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	AreaSphere->SetGenerateOverlapEvents(true);
	AreaSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));

	AreaSphere->SetHiddenInGame(true);
	AreaSphere->ShapeColor = FColor::Green;

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	ItemEffect->SetupAttachment(RootComponent);
	ItemEffect->SetHiddenInGame(false);
	ItemEffect->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	ItemEffect->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	ItemEffect->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	ItemEffect->SetVisibility(true);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	/* if (!AreaSphere)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ AreaSphere is NULL in actor: %s, class: %s"),
			*GetName(),
			*GetClass()->GetName());
		return;
	}*/

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetGenerateOverlapEvents(false);
	}
}

void AItem::EnableCustomDepth(bool bEnable)
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

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	
	// Prevent equippable weapons already in use from being set as overlapping
	if (IsA(AWeaponBase::StaticClass()))
	{
		AWeaponBase* Weapon = Cast<AWeaponBase>(this);
		if (Weapon && Weapon->ItemState == EItemState::EIS_Equipped) return;

		// Set OverlappingWeapon (and implicitly OverlappingItem if you wish)
		PickupInterface->SetOverlappingWeapon(Weapon);
	}
	else
	{
		// Set OverlappingItem for non-weapon item types
		PickupInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (!PickupInterface) return;

	// ⚠️ Don't clear if this item is already equipped
	if (ItemState == EItemState::EIS_Equipped) return;

	SetOwner(OtherActor);
	if (GetOwner()) return;

	PickupInterface->SetOverlappingItem(nullptr);
}

void AItem::SpawnPickupSystem()
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

void AItem::SpawnPickupSound()
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

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
	
}

void AItem::ShowPickupAndInfoWidgets(bool bShow)
{
	if (PickupGearWidgetComponent) PickupGearWidgetComponent->SetVisibility(bShow);
	if (ItemInfoWidgetComponent) ItemInfoWidgetComponent->SetVisibility(bShow);
}

