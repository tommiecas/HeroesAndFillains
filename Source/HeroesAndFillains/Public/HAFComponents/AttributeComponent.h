// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Interfaces/PickupInterface.h>

#include "CoreMinimal.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


class UHAFAttributeSet;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEROESANDFILLAINS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	void CharactersReceiveMeleeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	UPROPERTY() TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	void UseStamina(float StaminaCost);
	void RegenStamina(float DeltaTime);

	void UpdateTotalSouls(int32 NumberOfSouls);

	void UpdateTotalGold(int32 AmountOfGold);
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 GoldAcquired;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 SoulsGathered;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor Attributes")
	UHAFAttributeSet* AttributeSet;

public:
	FORCEINLINE bool IsCharacterAlive() const { return AttributeSet->GetHealth() > 0.f; }
	FORCEINLINE float GetHealth() const { return AttributeSet->GetHealth(); }
	FORCEINLINE float GetMaxHealth() const { return AttributeSet->GetMaxHealth(); }
	FORCEINLINE void SetHealth(float NewHealth) const{ AttributeSet->SetHealth(NewHealth); }
	FORCEINLINE void SetMaxHealth(float NewMaxHealth) const { AttributeSet->SetMaxHealth(NewMaxHealth); }

	FORCEINLINE float GetShield() const { return AttributeSet->GetShield(); }
	FORCEINLINE float GetMaxShield() const { return AttributeSet->GetMaxShield(); }
	FORCEINLINE float GetShieldPercent() const { return AttributeSet->GetShield() / AttributeSet->GetMaxShield(); }
	FORCEINLINE void SetShield(const float NewShield) const { AttributeSet->SetShield(NewShield); }
	FORCEINLINE void SetMaxShield(const float NewMaxShield) const { AttributeSet->SetMaxShield(NewMaxShield); }

	float GetStamina() const; 
	FORCEINLINE float GetMaxStamina() const { return AttributeSet->GetMaxStamina(); }
	FORCEINLINE float GetStaminaPercent() const { return AttributeSet->GetStamina() / AttributeSet->GetMaxStamina(); }
	FORCEINLINE void SetStamina(const float NewStamina) const { AttributeSet->SetStamina(NewStamina); }
	FORCEINLINE void SetMaxStamina(const float NewMaxStamina) const { AttributeSet->SetMaxStamina(NewMaxStamina); }

	FORCEINLINE float GetMajix() const  { return AttributeSet->GetMajix(); }
	FORCEINLINE float GetMaxMajix() const { return AttributeSet->GetMaxMajix(); }
	FORCEINLINE float GetMajixPercent() const { return AttributeSet->GetMajix() / AttributeSet->GetMaxMajix(); }
	FORCEINLINE void SetMajix(const float NewMajix) const { AttributeSet->SetMajix(NewMajix); }
	FORCEINLINE void SetMaxMajix(const float NewMaxMajix) const { AttributeSet->SetMaxMajix(NewMaxMajix); }
	
	FORCEINLINE float GetStaminaRegenRate() const { return StaminaRegenRate; }
	FORCEINLINE void SetStaminaRegenRate(float NewStaminaRegenRate) { StaminaRegenRate = NewStaminaRegenRate; }

	FORCEINLINE int32 GetGoldAcquired() const { return GoldAcquired; }
	FORCEINLINE void SetGoldAcquired(int32 NewGold) { GoldAcquired = NewGold; }

	FORCEINLINE int32 GetSoulsGathered() const { return SoulsGathered; }
	FORCEINLINE void SetSoulsGathered(int32 NewSouls) { SoulsGathered = NewSouls; }

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE void SetDodgeCost(float NewDodgeCost) { DodgeCost = NewDodgeCost; }


		
};