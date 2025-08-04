// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Interfaces/PickupInterface.h>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEROESANDFILLAINS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	void CharactersReceiveMeleeDamage(float Damage);
	void UseStamina(float StaminaCost);
	void RegenStamina(float DeltaTime);

	void UpdateTotalSouls(int32 NumberOfSouls);

	void UpdateTotalGold(int32 AmountOfGold);

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Shield;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxShield;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Majix;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxMajix;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 GoldAcquired;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 SoulsGathered;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;


public:
	FORCEINLINE bool IsCharacterAlive() { return Health > 0.f; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; };
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }
	FORCEINLINE void SetHealthPercent(float NewHealthPercent) { Health = NewHealthPercent * MaxHealth; }

	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE float GetShieldPercent() const { return Shield / MaxShield; }
	FORCEINLINE void SetShield(float NewShield) { Shield = NewShield; }
	FORCEINLINE void SetMaxShield(float NewMaxShield) { MaxShield = NewMaxShield; }
	FORCEINLINE void SetShieldPercent(float NewShieldPercent) { Shield = NewShieldPercent * MaxShield; }

	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }
	FORCEINLINE void SetStamina(float NewStamina) { Stamina = NewStamina; }
	FORCEINLINE void SetMaxStamina(float NewMaxStamina) { MaxStamina = NewMaxStamina; }
	FORCEINLINE void SetStaminaPercent(float NewStaminaPercent) { Stamina = NewStaminaPercent * MaxStamina; }

	FORCEINLINE float GetMajix() const  { return Majix; }
	FORCEINLINE float GetMaxMajix() const { return MaxMajix; }
	FORCEINLINE float GetMajixPercent() const { return Majix / MaxMajix; }
	FORCEINLINE void SetMajix(float NewMajix) { Majix = NewMajix; }
	FORCEINLINE void SetMaxMajix(float NewMaxMajix) { MaxMajix = NewMaxMajix; }
	FORCEINLINE void SetMajixPercent(float NewMajixPercent) { Majix = NewMajixPercent * MaxMajix; }

	FORCEINLINE float GetStaminaRegenRate() const { return StaminaRegenRate; }
	FORCEINLINE void SetStaminaRegenRate(float NewStaminaRegenRate) { StaminaRegenRate = NewStaminaRegenRate; }

	FORCEINLINE int32 GetGoldAcquired() const { return GoldAcquired; }
	FORCEINLINE void SetGoldAcquired(int32 NewGold) { GoldAcquired = NewGold; }

	FORCEINLINE int32 GetSoulsGathered() const { return SoulsGathered; }
	FORCEINLINE void SetSoulsGathered(int32 NewSouls) { SoulsGathered = NewSouls; }

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE void SetDodgeCost(float NewDodgeCost) { DodgeCost = NewDodgeCost; }


		
};