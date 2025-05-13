// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeapon.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EMeleeType : uint8
{
	EMT_None UMETA(DisplayName = "None"),

	EMT_ChaosSword UMETA(DisplayName = "Chaos Sword"),

	EMT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EMeleeState : uint8
{
	EMS_Initial UMETA(DisplayName = "Initial State"),
	EMS_Equipped UMETA(DisplayName = "Equipped State"),
	EMS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary State"),
	EMS_Dropped UMETA(DisplayName = "Dropped State"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};
UCLASS()
class HEROESANDFILLAINS_API AMeleeWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeleeWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void EnableCustomDepth(const bool bEnable) const;

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
	
	/*****************************
	***                        ***
	***   WEAPON INFORMATION   ***
	***                        ***
	*****************************/

	UPROPERTY(VisibleAnywhere, Category = "Melee Properties")
	class UWidgetComponent* MeleeInfoWidget1;

	UPROPERTY(VisibleAnywhere, Category = "Melee Properties")
	class UWidgetComponent* MeleeInfoWidget2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeWeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeWeaponHistory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeWeaponResistances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FText MeleeWeaponWeaknesses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	float MeleeWeaponDamage;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetA;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidgetB;
	
	UPROPERTY()
	class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Melee Properties")
	EMeleeType MeleeType = EMeleeType::EMT_None;

	void ShowPickupAndMeleeInfoWidgets(bool bShowPickupAndMeleeInfoWidgets);

	UPROPERTY(EditAnywhere, Category = "Melee Properties")
	class USoundCue* EquipSound;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeleeMesh;

	FText GetMeleeWeaponNameText() const;

	UPROPERTY(ReplicatedUsing = OnRep_MeleeState, VisibleAnywhere, Category = "Weapon Properties")
	EMeleeState MeleeState;

	UFUNCTION()
	void OnRep_MeleeState();


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

public:
	void SetMeleeState(EMeleeState State);

	

};

template<typename T>
inline T AMeleeWeapon::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

