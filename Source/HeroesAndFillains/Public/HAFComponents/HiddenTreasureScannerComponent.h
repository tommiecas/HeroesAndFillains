// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HiddenTreasureScannerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UHiddenTreasureScannerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UHiddenTreasureScannerComponent();

	// push 0..1 from the character’s Intuition attribute
	void SetIntuitionFraction(float Fraction);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnScanBegin (UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnScanEnd  (UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsLocal() const;
	
	// ---- tuning ----
	UPROPERTY(EditAnywhere, Category="Intuition")
	float BaseRadius = 800.f;           // default sense range (uu)

	UPROPERTY(EditAnywhere, Category="Intuition")
	float MaxBonusRadius = 1000.f;      // extra range at Intuition=1 (uu)



private:
	UPROPERTY()
	class USphereComponent* ScanSphere = nullptr;

	float CurrentIntuitionFraction = 0.f;
	void UpdateRadius() const;
	void ApplyToActorTreasures(AActor* Other, float Fraction);

};