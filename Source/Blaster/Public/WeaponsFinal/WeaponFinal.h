// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponFinal.generated.h"

UENUM(BlueprintType)
enum class EWeaponFinalState : uint8
{
	EWFS_Initial UMETA(DisplayName = "Initial State"),
	EWFS_Equipped UMETA(DisplayName = "Equipped State"),
	EWFS_Dropped UMETA(DisplayName = "Dropped State"),

	EWFS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeaponFinal : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponFinal();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowPickupWidget);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponFinalState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponFinalState WeaponFinalState;

	UFUNCTION()
	void OnRep_WeaponFinalState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

public:
	void SetWeaponFinalState(EWeaponFinalState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }



};
