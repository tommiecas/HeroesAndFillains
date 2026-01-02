// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PCPickupBaseItem.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "PrePackagedPCPickupItem.generated.h"

class UGameplayEffect;
class USphereComponent;

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API APrePackagedPCPickupItem : public APCPickupBaseItem
{
	GENERATED_BODY()
	
public:	
	APrePackagedPCPickupItem();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
	
protected:
	virtual void BeginPlay() override;


	template<typename T>
	T Avg(T First, T Second);


	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();
	
	/*********************************************
	****                                      ****
	****    ENABLE OR DISABLE CUSTOM DEPTH    ****
	****                                      ****
	*********************************************/

	virtual void EnableCustomDepth(bool bEnable) override;

		
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<UGameplayEffect> EffectToApply;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Khristel", meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* ItemMesh;
	

	virtual void ApplyPickupEffect_Implementation(class AFillainCharacter* PlayerChar);

private:

public:
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE UNiagaraComponent* GetItemEffect() const { return ItemEffect; }
	
};

template <typename T>
inline T APrePackagedPCPickupItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}