// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Components/SphereComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"

AHAFEffectActor::AHAFEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetGenerateOverlapEvents(false);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(200.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic); // Or your custom PCWeaponBox
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));

	AreaSphere->SetHiddenInGame(true);
	AreaSphere->ShapeColor = FColor::Green;

}

void AHAFEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: Change this to apply Gameplay Effect Const_Cast is a temporary hack.
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UHAFAttributeSet* HAFAttributes = Cast<UHAFAttributeSet>(Interface->GetAbilitySystemComponent()->GetAttributeSet(UHAFAttributeSet::StaticClass()));
		UHAFAttributeSet* MutableHAFAttributeSet = const_cast<UHAFAttributeSet*>(HAFAttributes);
		MutableHAFAttributeSet->SetHealth(HAFAttributes->GetHealth() + 25.f);
		Destroy();
	}
	
}

void AHAFEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AHAFEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AHAFEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AHAFEffectActor::EndOverlap);
	
}



