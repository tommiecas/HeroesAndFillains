// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEROESANDFILLAINS_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class AFillainCharacter;
	void Heal(float HealAmount, float HealingTime);
	void FortifyShield(float ShieldFortifyingAmount, float ShieldFortifyingTime);
	void Recharge(float StaminaRechargeAmount, float StaminaRechargeTime);
	void Summon(float MajixSummonedAmount, float MajixSummonedTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void ResetSpeed();
	void ResetJump();
	void SetInitialJumpVelocity(float JumpVelocity);	
	void SetInitialSpeed(float BaseSpeed, float CrouchSpeed);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);
	void StaminaRampUp(float DeltaTime);
	void MajixRampUp(float DeltaTime);

private:
	UPROPERTY()
	class AFillainCharacter* Character;

	/***********************
	***   HEALING BUFF   ***
	***********************/
	bool bAmIAlreadyHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	/***********************************
	***   REPLENISHING SHIELD BUFF   ***
	***********************************/
	bool bAmIAlreadyFortifyingShield = false;
	float FortifyingRate = 0;
	float AmountOfShieldFortified= 0.f;

	/***********************************
    ***   RECHARGING STAMINA  BUFF   ***
    ***********************************/
	bool bAmIAlreadyRechargingStamina = false;
	float RechargingRate = 0;
	float AmountOfStaminaRecharged = 0.f;

	/***********************************
    ***   SUMMONING MAJIX  BUFF   ***
    ***********************************/
	bool bAmIAlreadySummoningMajix = false;
	float SummoningRate = 0;
	float AmountOfMajixSummoned = 0.f;

	/*********************
	***   SPEED BUFF   ***
	*********************/
	FTimerHandle SpeedBuffTimer;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	/********************
	***   JUMP BUFF   ***
	********************/
	FTimerHandle JumpBuffTimer;
	float InitialJumpVelocity;
		
};