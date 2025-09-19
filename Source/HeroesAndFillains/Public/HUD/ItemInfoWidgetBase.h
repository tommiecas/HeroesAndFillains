// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfoWidgetBase.generated.h"

class UWidgetComponent;

UCLASS()
class HEROESANDFILLAINS_API UItemInfoWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// Required UObject constructor
	explicit UItemInfoWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	TObjectPtr<UWidgetComponent> ItemInfoOwningComponent;

	UFUNCTION()
	UWidgetComponent* GetItemInfoOwningComponent() const { return ItemInfoOwningComponent.Get(); }

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Line1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Line2;

	UPROPERTY()
	UItemInfoWidgetBase* InfoWidgetInstance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Line3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Line4;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Line5;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnimation;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnimation;

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Setup functions
	UFUNCTION(BlueprintCallable)
	void SetMeleeInfo(const FString& Name, const FString& History,
					  const FString& Resistances, const FString& Weaknesses,
					  const FString& Damage);

	UFUNCTION(BlueprintCallable)
	void SetRangedInfo(const FString& Name, const FString& Description,
					   const FString& Type, const FString& Rarity,
					   const FString& Damage);

protected:
	// Do setup that needs framework objects here, not in the constructor
	virtual void NativeConstruct() override;

	
public:
	FORCEINLINE UWidgetAnimation* GetFadeInAnimation() const { return FadeInAnimation; }
	FORCEINLINE UWidgetAnimation* GetFadeOutAnimation() const { return FadeOutAnimation; }
};