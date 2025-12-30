// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "HAFPlayerState.generated.h"

class ULevelUpInfo;
class UHAFAttributeSet;
class UHAFAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)
UCLASS()
class HEROESANDFILLAINS_API AHAFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHAFPlayerState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level Up Information")
	TObjectPtr<ULevelUpInfo> LevelUpInformation;
	

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

	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
	void AddLevel(const int32 LevelToAdd);
	void SetPlayerLevel(const int32 NewLevel);
	void AddXP(const int32 XPToAdd);
	void SetPlayerXP(const int32 NewXP);
	void AddAttributePoints(const float AttributePointsToAdd);
	void SetAttributePoints(const float NewAttributePoints);
	void AddSpellPoints(const float SpellPointsToAdd);
	void SetSpellPoints(const float NewSpellPoints);

	void UpdateAbilityCosts(int32 Level);

	UFUNCTION(BlueprintCallable)
	int32 GetFillainPlayerLevel() const { return Level; }

protected:
	// Strongly-typed, initialized in constructor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UHAFAbilitySystemComponent> HAFAbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UHAFAttributeSet> HAFAttributeSet = nullptr;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);


	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	float AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	float SpellPoints = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);


	UFUNCTION()
	void OnRep_AttributePoints(float OldAttributePoints) const;

	UFUNCTION()
	void OnRep_SpellPoints(float OldSpellPoints) const;


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
	FORCEINLINE int32 GetFillainPlayerXP() const { return XP; }
	FORCEINLINE float GetFillainPlayerAttributePoints() const { return AttributePoints; }
	FORCEINLINE float GetFillainPlayerSpellPoints() const { return SpellPoints; }
		
};