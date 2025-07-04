// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponTypes.h"
#include "Item.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering State"),
	EIS_Equipped UMETA(DisplayName = "Equipped State"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class HEROESANDFILLAINS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;


	virtual void ShowPickupAndInfoWidgets(bool bShow);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	class UWidgetComponent* PickupGearWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UWidgetComponent* ItemInfoWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "UI Properties")
	class UItemInfoWidgetBase* ItemInfoWidget;

	UPROPERTY(EditAnywhere, Category = "UI Properties")
	class UPickupGearWidget* PickupGearWidget;
	
	virtual void BeginPlay() override;
	
	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;
	
	UPROPERTY()
	class UItemInfoWidgetBase* ItemInfoWidgetInstance;
	
	/*********************************************
	****                                      ****
	****    ENABLE OR DISABLE CUSTOM DEPTH    ****
	****                                      ****
	*********************************************/

	virtual void EnableCustomDepth(bool bEnable);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UPickupGearWidget> PickupGearWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UItemInfoWidgetBase> ItemInfoWidgetClass;

	UPROPERTY()
	class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon and Pickup Types")
	EWeaponType WeaponType = EWeaponType::EWT_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon and Pickup Types")
	EMeleeType MeleeType = EMeleeType::EMT_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon and Pickup Types")
	ERangedType RangedType = ERangedType::ERT_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UNiagaraComponent* EmbersEffect;
	
protected:
	
private:
	
};

template <typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
