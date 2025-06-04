// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "Items/Item.h"
#include "WeaponBase.generated.h"

class UItemInfoWidgetBase;
class UPickupGearWidget;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Unclaimed UMETA(DisplayName = "Initial Unclaimed State"),
	EWS_EquippedOneHanded UMETA(DisplayName = "Equipped with a One-Handed Weapon State"),
	EWS_EquippedTwoHanded UMETA(DisplayName = "Equipped with a Two-Handed Weapon State"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary State"),
	EWS_Dropped UMETA(DisplayName = "Dropped State"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS(Blueprintable)
class HEROESANDFILLAINS_API AWeaponBase : public AItem
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState = EWeaponState::EWS_Unclaimed;

	UFUNCTION()
	void SetEquippedWeaponState();
	
	UFUNCTION()
	virtual void OnRep_WeaponState();

	virtual void WeaponDropped();

	bool bDestroyWeapon = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponProperties")
	TSubclassOf<AWeaponBase> WeaponClass;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class USoundCue* EquipSound;

	virtual void ShowPickupAndInfoWidgets(bool bShow) override;

	UPROPERTY()
	bool bIsEquipped = false;

	void SetOneOrTwoHandedWeapon(AWeaponBase* EquippedWeapon);


	UPROPERTY()
	AWeaponBase* OneHandedWeapon;

	UPROPERTY()
	AWeaponBase* TwoHandedWeapon;
	
protected:
	virtual void BeginPlay() override;
	
    	virtual void OnSphereOverlap(
    		UPrimitiveComponent* OverlappedComponent, 
    		AActor* OtherActor,
    		UPrimitiveComponent* OtherComp,
    		int32 OtherBodyIndex,
    		bool bFromSweep, 
    		const FHitResult& SweepResult);
	
    	virtual void OnSphereEndOverlap(
    		UPrimitiveComponent* OverlappedComponent,
    		AActor* OtherActor,
    		UPrimitiveComponent* OtherComp,
    		int32 OtherBodyIndex);
	
	template<typename T>
	static T Avg(T First, T Second);

	virtual void OnWeaponStateSet();
	virtual void OnEquippedOneHanded();
	virtual void OnEquippedTwoHanded();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();


	
	UPROPERTY()
	class AFillainCharacter* FillainOwnerCharacter;
	
	UPROPERTY()
	class AFillainPlayerController* FillainOwnerController;

	

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;
	
	UPROPERTY(EditAnywhere)
	ETeam Team;	
private:


public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE UWidgetComponent* GetItemInfoWidgetComponent() const { return ItemInfoWidgetComponent; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
};

template<typename T>
inline T AWeaponBase::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
