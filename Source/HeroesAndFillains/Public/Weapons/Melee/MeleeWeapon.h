// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponTypes.h"
#include "MeleeWeapon.generated.h"

class USphereComponent;

UCLASS()
class HEROESANDFILLAINS_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	AMeleeWeapon();
	virtual void EnableCustomDepth(bool bEnable) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void SetEquippedMeleeWeaponState();
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EMeleeType MeleeType = EMeleeType::EMT_None;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EMeleeType MeleeWeaponType = EMeleeType::EMT_None;
	
	UPROPERTY(EditAnywhere, Category = "UI Properties")
	UItemInfoWidgetBase* ItemInfoWidget;

	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnEquippedOneHanded() override;
	virtual void OnEquippedTwoHanded() override;
	virtual void OnDropped() override;
	virtual void OnEquippedSecondary() override;


private:
	void NativeConstruct();
	

	

public:
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeWeaponType; }
	FORCEINLINE UItemInfoWidgetBase* GetItemInfoWidget() const { return ItemInfoWidget; }
	

	

};



