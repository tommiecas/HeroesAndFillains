// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "HAFPlayerState.generated.h"

class UHAFAttributeSet;
class UHAFAbilitySystemComponent;

UCLASS()
class HEROESANDFILLAINS_API AHAFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHAFPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Convenience getters
	UFUNCTION(BlueprintCallable)
	UHAFAbilitySystemComponent* GetHAFAbilitySystemComponent() const { return HAFAbilitySystemComponent; }
	UFUNCTION(BlueprintCallable)
	UHAFAttributeSet* GetHAFAttributeSet() const { return HAFAttributeSet; }

	// Legacy base-class getters if you still use them
	UFUNCTION(BlueprintCallable)
	UAttributeSet* GetAttributeSet() const { return HAFAttributeSet; }

	/*
	** Replication Notifies
	*/
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	UFUNCTION()
	virtual void OnRep_Defeats();

protected:
	// Strongly-typed, initialized in constructor
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHAFAbilitySystemComponent> HAFAbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UHAFAttributeSet> HAFAttributeSet = nullptr;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class AFillainCharacter* Character = nullptr;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class AFillainPlayerController* Controller = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE AFillainCharacter* GetFillainPlayerCharacter() const { return Character; }
	FORCEINLINE AFillainPlayerController* GetFillainPlayerController() const { return Controller; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
};