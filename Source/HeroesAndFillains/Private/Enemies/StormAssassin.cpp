// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StormAssassin.h"
#include "Animation/AnimInstance.h"
#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/Khymeyrra.h"
#include "GameFramework/CharacterMovementComponent.h"
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

void AStormAssassin::Die()
{
	Super::Die();
}

void AStormAssassin::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();
	Dissolve();
}

void AStormAssassin::Dissolve()
{
	if (IsValid(DissolveMaterialInstanceZero) && IsValid(DissolveMaterialInstanceOne) && IsValid(DissolveMaterialInstanceTwo) && IsValid(DissolveMaterialInstanceThree) && IsValid(DissolveMaterialInstanceFour) && IsValid(DissolveMaterialInstanceFive) && IsValid(DissolveMaterialInstanceSix) && IsValid(DissolveMaterialInstanceSeven) && IsValid(DissolveMaterialInstanceEight) && IsValid(DissolveMaterialInstanceNine))
	{
		UMaterialInstanceDynamic* DynamicStormMatInstZero = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceZero, this);
		UMaterialInstanceDynamic* DynamicStormMatInstOne = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceOne, this);
		UMaterialInstanceDynamic* DynamicStormMatInstTwo = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceTwo, this);
		UMaterialInstanceDynamic* DynamicStormMatInstThree = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceThree, this);
		UMaterialInstanceDynamic* DynamicStormMatInstFour = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceFour, this);
		UMaterialInstanceDynamic* DynamicStormMatInstFive = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceFive, this);
		UMaterialInstanceDynamic* DynamicStormMatInstSix = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceSix, this);
		UMaterialInstanceDynamic* DynamicStormMatInstSeven = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceSeven, this);
		UMaterialInstanceDynamic* DynamicStormMatInstEight = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceEight, this);
		UMaterialInstanceDynamic* DynamicStormMatInstNine = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceNine, this);

		GetMesh()->SetMaterial(0, DynamicStormMatInstZero);
		GetMesh()->SetMaterial(1, DynamicStormMatInstOne);
		GetMesh()->SetMaterial(2, DynamicStormMatInstTwo);
		GetMesh()->SetMaterial(3, DynamicStormMatInstThree);
		GetMesh()->SetMaterial(4, DynamicStormMatInstFour);
		GetMesh()->SetMaterial(5, DynamicStormMatInstFive);
		GetMesh()->SetMaterial(6, DynamicStormMatInstSix);
		GetMesh()->SetMaterial(7, DynamicStormMatInstSeven);
		GetMesh()->SetMaterial(8, DynamicStormMatInstEight);
		GetMesh()->SetMaterial(9, DynamicStormMatInstNine);
		
		StartStormDissolveTimelineZero(DynamicStormMatInstZero);
		StartStormDissolveTimelineOne(DynamicStormMatInstOne);
		StartStormDissolveTimelineTwo(DynamicStormMatInstTwo);
		StartStormDissolveTimelineThree(DynamicStormMatInstThree);
		StartStormDissolveTimelineFour(DynamicStormMatInstFour);
		StartStormDissolveTimelineFive(DynamicStormMatInstFive);
		StartStormDissolveTimelineSix(DynamicStormMatInstSix);
		StartStormDissolveTimelineSeven(DynamicStormMatInstSeven);
		StartStormDissolveTimelineEight(DynamicStormMatInstEight);
		StartStormDissolveTimelineNine(DynamicStormMatInstNine);
	}
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