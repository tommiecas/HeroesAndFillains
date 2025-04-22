// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFinal/CasingFinal.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasingFinal::ACasingFinal()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	CasingMesh->SetEnableGravity(true);

	ShellEjectionImpulse = 20.f;
}

void ACasingFinal::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasingFinal::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	
}

void ACasingFinal::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CasingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
	}
	Destroy();
}

