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
	void Equip(USceneComponent* InParent, FName InSocketName);
	virtual void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName) override;
	
	UFUNCTION()
	void SetEquippedMeleeWeaponState();

	UFUNCTION(BlueprintCallable)
	void SetMeleeWeaponInformationText(UWidgetComponent* MeleeWidgetComponent, AMeleeWeapon* MeleeWeapon);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponHistory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponResistances;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponWeaknesses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponDamage;

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
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeType; }
	FORCEINLINE UItemInfoWidgetBase* GetItemInfoWidget() const { return ItemInfoWidget; }
	

	

};



