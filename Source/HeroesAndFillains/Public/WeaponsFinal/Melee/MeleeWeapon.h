// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponsFinal/WeaponBase.h"
#include "MeleeWeapon.generated.h"

class USphereComponent;

UENUM(Blueprintable, BlueprintType)
enum class EMeleeType : uint8
{
	EMT_None UMETA(DisplayName = "None"),

	EMT_ChaosSword UMETA(DisplayName = "Chaos Sword"),

	EMT_RubySword UMETA(DisplayName = "Ruby Sword"),

	EMT_SapphireSword UMETA(DisplayName = "Sapphire Sword"),

	EMT_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class HEROESANDFILLAINS_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	AMeleeWeapon();
	virtual void EnableCustomDepth(bool bEnable) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EMeleeType MeleeType = EMeleeType::EMT_None;
	
	/*****************************
	***                        ***
	***   WEAPON INFORMATION   ***
	***                        ***
	*****************************/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponHistoryText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponResistancesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponWeaknessesText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	float MeleeWeaponDamageText;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EMeleeType MeleeWeaponType = EMeleeType::EMT_None;

	virtual void ShowPickupAndInfoWidgets(bool bShowPickupAndMeleeInfoWidgets) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	virtual void OnEquippedSecondary() override;


private:
	

public:
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeWeaponType; }
	FText GetMeleeWeaponNameText();
	FORCEINLINE FText GetMeleeWeaponHistoryText() const { return MeleeWeaponHistoryText; }
	FORCEINLINE FText GetMeleeWeaponResistancesText() const { return MeleeWeaponResistancesText; }
	FORCEINLINE FText GetMeleeWeaponWeaknessesText() const { return MeleeWeaponWeaknessesText; }
	FORCEINLINE float GetMeleeWeaponDamageText() const { return MeleeWeaponDamageText; }

	

	

};



