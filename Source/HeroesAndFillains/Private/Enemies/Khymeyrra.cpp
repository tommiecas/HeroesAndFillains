// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Khymeyrra.h"

#include "Characters/FillainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeroesAndFillains/HeroesAndFillains.h"


AKhymeyrra::AKhymeyrra()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RightAxeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightAxeCollision"));
	RightAxeCollision->SetupAttachment(GetMesh(), FName("RightAxeSocket"));
	RightAxeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightAxeCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	RightAxeCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightAxeCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	RightAxeCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap); // Player Character's Weapon
	RightAxeCollision->SetGenerateOverlapEvents(true);

	LeftAxeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftAxeCollision"));
	LeftAxeCollision->SetupAttachment(GetMesh(), FName("LeftAxeSocket"));
	LeftAxeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftAxeCollision->SetCollisionObjectType(ECC_EnemyWeaponBox); // e.g., Enemy
	LeftAxeCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftAxeCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap); // PlayerCharacter
	LeftAxeCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECollisionResponse::ECR_Overlap); // Player Character's Weapon
	LeftAxeCollision->SetGenerateOverlapEvents(true);

	EnemyDisplayName = TEXT("a vicious Khymeyrra");
}

void AKhymeyrra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKhymeyrra::AttackEnd()
{
	Super::AttackEnd();

	DisableLeftAxe();
	DisableRightAxe();
}

void AKhymeyrra::BeginPlay()
{
	Super::BeginPlay();

	RightAxeCollision->OnComponentBeginOverlap.AddDynamic(this, &AKhymeyrra::OnRightAxeOverlap);
	LeftAxeCollision->OnComponentBeginOverlap.AddDynamic(this, &AKhymeyrra::OnLeftAxeOverlap);

	FTransform RootBone = GetMesh()->GetSocketTransform(FName("root"), RTS_World);
	UE_LOG(LogTemp, Warning, TEXT("Root bone location: %s, scale: %s"), 
		*RootBone.GetLocation().ToString(), 
		*RootBone.GetScale3D().ToString());

	UE_LOG(LogTemp, Warning, TEXT("Mesh location: %s, scale: %s"),
	*GetMesh()->GetComponentLocation().ToString(),
	*GetMesh()->GetComponentScale().ToString());
}

void AKhymeyrra::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();
	Dissolve();
}

void AKhymeyrra::Dissolve()
{
	if (IsValid(KhymeyrrahDissolveMaterialInstanceZero) && IsValid(KhymeyrrahDissolveMaterialInstanceOne) && IsValid(KhymeyrrahDissolveMaterialInstanceTwo) && IsValid(KhymeyrrahDissolveMaterialInstanceThree) && IsValid(KhymeyrrahDissolveMaterialInstanceFour) && IsValid(KhymeyrrahDissolveMaterialInstanceFive) && IsValid(KhymeyrrahDissolveMaterialInstanceSix) && IsValid(KhymeyrrahDissolveMaterialInstanceSeven))
	{
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstZero = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceZero, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstOne = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceOne, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstTwo = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceTwo, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstThree = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceThree, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstFour = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceFour, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstFive = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceFive, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstSix = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceSix, this);
		UMaterialInstanceDynamic* DynamicKhymeyrrahMatInstSeven = UMaterialInstanceDynamic::Create(KhymeyrrahDissolveMaterialInstanceSeven, this);

		GetMesh()->SetMaterial(0, DynamicKhymeyrrahMatInstZero);
		GetMesh()->SetMaterial(1, DynamicKhymeyrrahMatInstOne);
		GetMesh()->SetMaterial(2, DynamicKhymeyrrahMatInstTwo);
		GetMesh()->SetMaterial(3, DynamicKhymeyrrahMatInstThree);
		GetMesh()->SetMaterial(4, DynamicKhymeyrrahMatInstFour);
		GetMesh()->SetMaterial(5, DynamicKhymeyrrahMatInstFive);
		GetMesh()->SetMaterial(6, DynamicKhymeyrrahMatInstSix);
		GetMesh()->SetMaterial(7, DynamicKhymeyrrahMatInstSeven);
		
		StartKhymeyrrahDissolveTimelineZero(DynamicKhymeyrrahMatInstZero);
		StartKhymeyrrahDissolveTimelineOne(DynamicKhymeyrrahMatInstOne);
		StartKhymeyrrahDissolveTimelineTwo(DynamicKhymeyrrahMatInstTwo);
		StartKhymeyrrahDissolveTimelineThree(DynamicKhymeyrrahMatInstThree);
		StartKhymeyrrahDissolveTimelineFour(DynamicKhymeyrrahMatInstFour);
		StartKhymeyrrahDissolveTimelineFive(DynamicKhymeyrrahMatInstFive);
		StartKhymeyrrahDissolveTimelineSix(DynamicKhymeyrrahMatInstSix);
		StartKhymeyrrahDissolveTimelineSeven(DynamicKhymeyrrahMatInstSeven);
	}
}

int32 AKhymeyrra::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);;
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void AKhymeyrra::OnLeftAxeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !LeftAxeDamagedActors.Contains(Player))
	{
		LeftAxeDamagedActors.Add(Player);
		
		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, AxeDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		AxeDamageResetTimer,
		this,
		&AKhymeyrra::DisableRightAxe,
		0.25f, // or however long the kick takes
		false);
}

void AKhymeyrra::OnRightAxeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* Player = Cast<AFillainCharacter>(OtherActor);
	if (Player && bCanDamage && !RightAxeDamagedActors.Contains(Player))
	{
		RightAxeDamagedActors.Add(Player);
		
		if (const UDamageType* DamageTypeInstance = NewObject<UDamageType>(this, UDamageType::StaticClass()))
		{
			Player->ReceiveDamage(Player, AxeDamage, DamageTypeInstance, GetController(), this);
		}
		bCanDamage = false; // or timer reset, etc.
	}
	GetWorld()->GetTimerManager().SetTimer(
		AxeDamageResetTimer,
		this,
		&AKhymeyrra::DisableLeftAxe,
		0.25f, // or however long the kick takes
		false);
}

void AKhymeyrra::EnableLeftAxe()
{
	LeftAxeDamagedActors.Empty();
	EnemyState = EEnemyState::EES_Attacking;
	bCanDamage = true;
	CanAttack();
	StopAllMontages();
	LeftAxeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftAxeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftAxeCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap); // PlayerCharacter
	LeftAxeCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
	LeftAxeCollision->SetGenerateOverlapEvents(true);
}

void AKhymeyrra::DisableLeftAxe()
{
	LeftAxeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftAxeCollision->SetGenerateOverlapEvents(false);
	EnemyState = EEnemyState::EES_Patrolling;
	bCanDamage = false;
	StopAllMontages();
	LeftAxeDamagedActors.Empty(); // ✅ Clear after swing ends
}

void AKhymeyrra::EnableRightAxe()
{
	RightAxeDamagedActors.Empty();
	EnemyState = EEnemyState::EES_Attacking;
	bCanDamage = true;
	CanAttack();
	StopAllMontages();
	RightAxeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightAxeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightAxeCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap); // PlayerCharacter
	RightAxeCollision->SetCollisionResponseToChannel(ECC_PCWeaponBox, ECR_Overlap);
	RightAxeCollision->SetGenerateOverlapEvents(true);
}

void AKhymeyrra::DisableRightAxe()
{
	RightAxeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightAxeCollision->SetGenerateOverlapEvents(false);
	bCanDamage = false;
	EnemyState = EEnemyState::EES_Patrolling;
	StopAllMontages();
	RightAxeDamagedActors.Empty(); // ✅ Clear after swing ends
}