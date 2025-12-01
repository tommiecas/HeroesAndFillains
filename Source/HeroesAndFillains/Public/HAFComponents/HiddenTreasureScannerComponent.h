// HiddenTreasureScannerComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HiddenTreasureScannerComponent.generated.h"

class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UHiddenTreasureScannerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHiddenTreasureScannerComponent();

	// Push 0..1 from the character's Intuition attribute
	void SetIntuitionFraction(float Fraction);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnScanBegin(
		UPrimitiveComponent* PrimitiveComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnScanEnd(
		UPrimitiveComponent* PrimitiveComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	bool IsLocal() const;

	// ---- tuning ----
	UPROPERTY(EditAnywhere, Category="Intuition")
	float BaseRadius = 800.f; // default sense range

	UPROPERTY(EditAnywhere, Category="Intuition")
	float MaxBonusRadius = 1000.f; // range bonus at Intuition=1

private:

	// Created by the component, not exposed or serialized
	UPROPERTY(Transient)
	USphereComponent* ScanSphere = nullptr;

	float CurrentIntuitionFraction = 0.f;

	void UpdateRadius() const;
	void ApplyToActorTreasures(AActor* Other, float Fraction);
};
