// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StormAssassin.h"
#include "Animation/AnimInstance.h"
#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/Khymeyrra.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/EnemyHealthBarWidgetComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "Weapons/Melee/StormWeapons.h"

AStormAssassin::AStormAssassin()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RightFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFootCollision"));
	RightFootCollision->SetupAttachment(GetMesh(), FName("RightFootSocket"));
	RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	RightFootCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	RightFootCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap); // Player Character's Weapon
	RightFootCollision->SetGenerateOverlapEvents(true);

	LeftFootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFootCollision"));
	LeftFootCollision->SetupAttachment(GetMesh(), FName("LeftFootSocket"));
	LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	LeftFootCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftFootCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	LeftFootCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap); // Player Character's Weapon
	LeftFootCollision->SetGenerateOverlapEvents(true);

	if (StormAssassin == EStormAssassin::ESA_Sandstorm) EnemyDisplayName = TEXT("the Storm Assassin known as Sandstorm");
	if (StormAssassin == EStormAssassin::ESA_Soulstorm) EnemyDisplayName = TEXT("the Storm Assassin known as Soulstorm");
	if (StormAssassin == EStormAssassin::ESA_Skystorm) EnemyDisplayName = TEXT("the Storm Assassin known as Skystorm");
	if (StormAssassin == EStormAssassin::ESA_Shadowstorm) EnemyDisplayName = TEXT("the Storm Assassin known as Shadowstorm");
}

void AStormAssassin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStormAssassin::AttackEnd()
{
	Super::AttackEnd();

	DisableLeftFoot();
	DisableRightFoot();
}

void AStormAssassin::BeginPlay()
{
	Super::BeginPlay();

	RightFootCollision->OnComponentBeginOverlap.AddDynamic(this, &AStormAssassin::OnRightFootOverlap);
	LeftFootCollision->OnComponentBeginOverlap.AddDynamic(this, &AStormAssassin::OnLeftFootOverlap);
}

void AStormAssassin::OnLeftFootOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !LeftFootDamagedActors.Contains(Player))
	{
		LeftFootDamagedActors.Add(Player);
		
		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, FootDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		FootDamageResetTimer,
		this,
		&AStormAssassin::DisableRightFoot,
		0.25f, // or however long the kick takes
		false);
}

void AStormAssassin::OnRightFootOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !RightFootDamagedActors.Contains(Player))
	{
		RightFootDamagedActors.Add(Player);
		
		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, FootDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		FootDamageResetTimer,
		this,
		&AStormAssassin::DisableLeftFoot,
		0.25f, // or however long the kick takes
		false);
}

void AStormAssassin::EnableLeftFoot()
{
	LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftFootDamagedActors.Empty();
	bCanDamage = true;
	EnemyState = EEnemyState::EES_Attacking;
	CanAttack();
	StopAllMontages();
	// UE_LOG(LogTemp, Warning, TEXT("🟢 Left foot collision enabled"));
}

void AStormAssassin::DisableLeftFoot()
{
	LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootCollision->SetGenerateOverlapEvents(false);
	bCanDamage = false;
	EnemyState = EEnemyState::EES_Patrolling;
	StopAllMontages();
	LeftFootDamagedActors.Empty(); // ✅ Clear after swing ends
}

void AStormAssassin::EnableRightFoot()
{
	RightFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightFootDamagedActors.Empty();
	bCanDamage = true;
	EnemyState = EEnemyState::EES_Patrolling;
	CanAttack();
	StopAllMontages();
}

void AStormAssassin::DisableRightFoot()
{
	RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootCollision->SetGenerateOverlapEvents(false);
	bCanDamage = false;
	StopAllMontages();
	RightFootDamagedActors.Empty(); // ✅ Clear after swing ends
}

int32 AStormAssassin::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);;
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}