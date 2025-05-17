// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponsFinal/WeaponBase.h"
#include "MeleeWeapon.generated.h"

class USphereComponent;

UENUM(BlueprintType)
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

	UPROPERTY(VisibleAnywhere, Category = "Melee Properties")
	class UMeleeInfoWidgetComponent* MeleeInfoWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Melee Properties")
	class UMeleeInfoWidgetComponent* MeleeInfoWidget2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponHistory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponResistances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	FText MeleeWeaponWeaknesses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	float MeleeWeaponDamage;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EMeleeType MeleeWeaponType = EMeleeType::EMT_None;

	virtual void ShowPickupAndInfoWidgets(bool bShowPickupAndMeleeInfoWidgets) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	FText GetMeleeWeaponNameText() const;

	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	virtual void OnEquippedSecondary() override;


private:
	

public:
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeWeaponType; }

	

	

};



