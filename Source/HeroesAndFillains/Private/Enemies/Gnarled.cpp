// Fill out your copyright notice in the Description page of Project Settings.



#include "Enemies/Gnarled.h"

#include "Components/CapsuleComponent.h"
#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "Animation/AnimInstance.h"
#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "GameFramework/DamageType.h"

AGnarled::AGnarled()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RightFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollision"));
	RightFistCollision->SetupAttachment(GetMesh(), FName("RightFistSocket"));
	RightFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	RightFistCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	RightFistCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
	RightFistCollision->SetGenerateOverlapEvents(true);

	LeftFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistCollision"));
	LeftFistCollision->SetupAttachment(GetMesh(), FName("LeftFistSocket"));
	LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	LeftFistCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	LeftFistCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap);
	LeftFistCollision->SetGenerateOverlapEvents(true);

	if (bIsGnarledMale) EnemyDisplayName = TEXT("a male Gnarled");
	else EnemyDisplayName = TEXT("a female Gnarled");
}

void AGnarled::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGnarled::BeginPlay()
{
	Super::BeginPlay();

	RightFistCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnarled::OnFistOverlap);
	LeftFistCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnarled::OnFistOverlap);
}

void AGnarled::OnFistOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !RightFistDamagedActors.Contains(Player) && !LeftFistDamagedActors.Contains(Player))
	{
		RightFistDamagedActors.Add(Player);
		LeftFistDamagedActors.Add(Player);
		
		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, FistDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		FistDamageResetTimer,
		this,
		&AGnarled::ResetCanDamage,
		0.25f, // or however long the punch takes
		false
	);
}

void AGnarled::EnableLeftFist()
{
	ResetCanDamage();
	LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftFistDamagedActors.Empty();
	EnemyState = EEnemyState::EES_Patrolling;
	CanAttack();
	StopAllMontages();
	UE_LOG(LogTemp, Warning, TEXT("🟢 Left fist collision enabled"));
}

void AGnarled::DisableLeftFist()
{
	LeftFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bCanDamage = false;
	StopAllMontages();
	LeftFistDamagedActors.Empty(); // ✅ Clear after swing ends
	UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared at end of fist swing"));
}

void AGnarled::EnableRightFist()
{
	ResetCanDamage();
	RightFistCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightFistDamagedActors.Empty(); // or RightFistDamagedActors if using separate lists
	EnemyState = EEnemyState::EES_Patrolling;
	CanAttack();
	StopAllMontages();
	UE_LOG(LogTemp, Warning, TEXT("🟢 Right fist collision enabled"));
}

void AGnarled::DisableRightFist()
{
	RightFistCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAllMontages();
	RightFistDamagedActors.Empty(); // ✅ Clear after swing ends
	UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared at end of fist swing"));
}

void AGnarled::ResetCanDamage()
{
	bCanDamage = true;
}

int32 AGnarled::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);;
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void AGnarled::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (RightFistCollision && LeftFistCollision)
	{
		RightFistCollision->SetCollisionEnabled(CollisionEnabled);
		RightFistCollision->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);
		LeftFistCollision->SetCollisionEnabled(CollisionEnabled);
		LeftFistCollision->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);
	}

	if (CollisionEnabled == ECollisionEnabled::NoCollision)
	{
		LeftFistDamagedActors.Empty();
		RightFistDamagedActors.Empty();
	}
}