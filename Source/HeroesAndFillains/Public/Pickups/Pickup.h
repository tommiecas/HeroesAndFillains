// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/PrePackagedPCPickupItem.h"
#include "Pickup.generated.h"


class UWidgetComponent;

UCLASS()
class HEROESANDFILLAINS_API APickup : public APrePackagedPCPickupItem
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual void EnableCustomDepth(bool bEnable) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY()
	UWidgetComponent* PickupWidgetComponent;

	UPROPERTY()
	UWidgetComponent* InfoWidgetComponent;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(
		class UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	virtual void OnSphereEndOverlap(
		class UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;

	FTimerHandle BindOverlapTimer;
	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished();

public:
	FORCEINLINE UStaticMeshComponent* GetPickupMesh() const { return PickupMesh; }



};
