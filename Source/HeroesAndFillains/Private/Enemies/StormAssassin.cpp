// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StormAssassin.h"
#include "Animation/AnimInstance.h"
#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarWidgetComponent.h"
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

void AStormAssassin::BeginPlay()
{
	Super::BeginPlay();

	RightFootCollision->OnComponentBeginOverlap.AddDynamic(this, &AStormAssassin::OnFootOverlap);
	LeftFootCollision->OnComponentBeginOverlap.AddDynamic(this, &AStormAssassin::OnFootOverlap);
}

void AStormAssassin::OnFootOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !RightFootDamagedActors.Contains(Player) && !LeftFootDamagedActors.Contains(Player))
	{
		RightFootDamagedActors.Add(Player);
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
		&AStormAssassin::ResetCanDamage,
		0.25f, // or however long the kick takes
		false
	);
}

void AStormAssassin::EnableLeftFoot()
{
	ResetCanDamage();
	LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftFootDamagedActors.Empty();
	EnemyState = EEnemyState::EES_Patrolling;
	CanAttack();
	StopAllMontages();
	UE_LOG(LogTemp, Warning, TEXT("🟢 Left foot collision enabled"));
}

void AStormAssassin::DisableLeftFoot()
{
	LeftFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bCanDamage = false;
	StopAllMontages();
	LeftFootDamagedActors.Empty(); // ✅ Clear after swing ends
	UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared at end of left foot kick"));
}

void AStormAssassin::EnableRightFoot()
{
	ResetCanDamage();
	RightFootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightFootDamagedActors.Empty();
	EnemyState = EEnemyState::EES_Patrolling;
	CanAttack();
	StopAllMontages();
	UE_LOG(LogTemp, Warning, TEXT("🟢 Right foot collision enabled"));
}

void AStormAssassin::DisableRightFoot()
{
	RightFootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bCanDamage = false;
	StopAllMontages();
	RightFootDamagedActors.Empty(); // ✅ Clear after swing ends
	UE_LOG(LogTemp, Warning, TEXT("🧹 DamagedActors list cleared at end of right foot kick"));
}

void AStormAssassin::ResetCanDamage()
{
	bCanDamage = true;
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

void AStormAssassin::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (RightFootCollision && LeftFootCollision)
	{
		RightFootCollision->SetCollisionEnabled(CollisionEnabled);
		RightFootCollision->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);
		LeftFootCollision->SetCollisionEnabled(CollisionEnabled);
		LeftFootCollision->SetGenerateOverlapEvents(CollisionEnabled == ECollisionEnabled::QueryOnly);
	}

	if (CollisionEnabled == ECollisionEnabled::NoCollision)
	{
		LeftFootDamagedActors.Empty();
		RightFootDamagedActors.Empty();
	}
}






