// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "HAFAbilityTypes.h"
#include "Weapons/WeaponBase.h"
#include "MajixWeapon.generated.h"

class UHAFUserWidget;
class UBoxComponent;

UCLASS()
class HEROESANDFILLAINS_API AMajixWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AMajixWeapon();

	UFUNCTION(BlueprintCallable)
	void Equip(USkeletalMeshComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Component")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Majix Type")
	EMaJixType MaJixType = EMaJixType::EJT_MAX;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> NewSphere;

	UFUNCTION(BlueprintCallable)
	AMajixWeapon* InitializeMajixWeaponMeshIfNotProjectile();

	UFUNCTION(BlueprintCallable)
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);

	void SetEquippedWeaponState();
	void OnEquippedOneHanded();
	void OnEquippedTwoHanded();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* HoveringLight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* HoveringDecal = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* PickupWidgetComponent = nullptr;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* InfoWidgetComponent = nullptr;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHAFUserWidget* InfoWidget = nullptr;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHAFUserWidget* PickupWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UPickupGearWidget> PickupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UItemInfoWidgetBase> InfoWidgetClass;

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* WeaponBox; 

private:

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE void SetWeaponMesh(USkeletalMeshComponent* NewWeaponMesh) { WeaponMesh = NewWeaponMesh; }
	FORCEINLINE USphereComponent* GetNewSphere() const { return NewSphere; }
	FORCEINLINE void SetNewSphere(USphereComponent* NewestSphere) { NewSphere = NewestSphere; }
public:

};
