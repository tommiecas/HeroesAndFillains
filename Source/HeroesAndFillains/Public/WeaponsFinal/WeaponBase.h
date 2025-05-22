// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "WeaponBase.generated.h"

class UItemInfoWidgetBase;
class UPickupGearWidget;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped State"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary State"),
	EWS_Dropped UMETA(DisplayName = "Dropped State"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS(Blueprintable)
class HEROESANDFILLAINS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	virtual void OnRep_WeaponState();
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	virtual void WeaponDropped();

	bool bDestroyWeapon = false;
	
	/*****************************
	***                        ***
	***   WEAPON INFORMATION   ***
	***                        ***
	*****************************/
	
	// Floating hover parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldHover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverAmplitude = 20.f; // How far it moves up/down (units)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverSpeed = 2.f; // How fast it oscillates

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UPointLightComponent* HoverLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	class UDecalComponent* HoverDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldFloatSpin = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* PickupGearWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* PickupGearWidgetComponentB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponentB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UPickupGearWidget> PickupGearWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ItemInfoWidgetClass;
	
	UPROPERTY()
	class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;

	virtual void ShowPickupAndInfoWidgets(bool bShowPickupAndInfoWidgets);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class USoundCue* EquipSound;

	/*********************************************
	****                                      ****
	****    ENABLE OR DISABLE CUSTOM DEPTH    ****
	****                                      ****
	*********************************************/

	virtual void EnableCustomDepth(bool bEnable);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
    	virtual void OnSphereOverlap(
    		UPrimitiveComponent* OverlappedComponent, 
    		AActor* OtherActor,
    		UPrimitiveComponent* OtherComp,
    		int32 OtherBodyIndex,
    		bool bFromSweep, 
    		const FHitResult& SweepResult);
    
    	UFUNCTION(BlueprintCallable)
    	virtual void OnSphereEndOverlap(
    		UPrimitiveComponent* OverlappedComponent,
    		AActor* OtherActor,
    		UPrimitiveComponent* OtherComp,
    		int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;
	
	UFUNCTION(BlueprintPure)
	float TransformedSin() const;
	
	UFUNCTION(BlueprintPure)
	float TransformedCos() const;
	
	template<typename T>
	static T Avg(T First, T Second);

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

	UPROPERTY()
	class AFillainCharacter* FillainOwnerCharacter;
	
	UPROPERTY()
	class AFillainPlayerController* FillainOwnerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

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
	FORCEINLINE UWidgetComponent* GetPickupWidgetComponentA() const { return PickupGearWidgetComponentA; }
	FORCEINLINE UWidgetComponent* GetPickupWidgetComponentB() const { return PickupGearWidgetComponentB; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE UWidgetComponent* GetItemInfoWidgetComponentA() const { return ItemInfoWidgetComponentA; }
	FORCEINLINE UWidgetComponent* GetItemInfoWidgetComponentB() const { return ItemInfoWidgetComponentB; }
};

template<typename T>
inline T AWeaponBase::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
