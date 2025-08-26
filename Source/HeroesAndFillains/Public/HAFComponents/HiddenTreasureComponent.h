// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
#include "GameplayEffectTypes.h"
#include "HiddenTreasureComponent.generated.h"

class UAbilitySystemComponent;
class UHAFAttributeSet;        // <-- your attribute set with Intuition
class UShapeComponent;


UCLASS(BlueprintType, Blueprintable, ClassGroup=(Treasure), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UHiddenTreasureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHiddenTreasureComponent();

	// Called by the player scanner: 0..1 where 1 = max glow
	void ApplyIntuitionScale(float Fraction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Highlight")
	TObjectPtr<UMeshComponent> ExplicitHighlightMesh = nullptr;   // optional: set in BP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Highlight")
	FName PreferredHighlightTag = TEXT("Highlight");     

protected:
	UMeshComponent* FindBestMesh() const;                          // <- implement below

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Material param driving emissive
	UPROPERTY(EditAnywhere, Category="Glow")
	FName GlowParamName = TEXT("GlowIntensity");

	// Mapping 0..1 -> emissive intensity
	UPROPERTY(EditAnywhere, Category="Glow")
	float MinGlow = 0.0f;

	UPROPERTY(EditAnywhere, Category="Glow")
	float MaxGlow = 25.0f; // raise if you want stronger bloom

	// Smoothing
	UPROPERTY(EditAnywhere, Category="Glow")
	float FadeSpeed = 6.0f;

	// Optional response curve (1 = linear, >1 = slower start, faster end)
	UPROPERTY(EditAnywhere, Category="Glow")
	float Gamma = 1.25f;

private:
	void BuildMIDs();

	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> GlowMIDs;

	float TargetGlow = 0.f;
	float CurrentGlow = 0.f;
	bool bBuilt = false;
};