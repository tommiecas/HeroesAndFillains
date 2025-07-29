// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Team.h"
#include "HAFPlayerState.generated.h"


class UHAFAttributeSet;
class UHAFAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AHAFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHAFPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	UHAFAbilitySystemComponent* GetHAFAbilitySystemComponent() const { return HAFAbilitySystemComponent; };
	UHAFAttributeSet* GetHAFAttributeSet() const { return HAFAttributeSet; };
	
	/*
	** Replication Notifies
	*/
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	UFUNCTION()
	virtual void OnRep_Defeats();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<UHAFAttributeSet> HAFAttributeSet;

	UPROPERTY()
	TObjectPtr<UHAFAbilitySystemComponent> HAFAbilitySystemComponent;

protected:

private:
	UPROPERTY(meta = (AllowPrvateAccess = "true"))
	class AFillainCharacter* Character;
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class AFillainPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats);
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE AFillainCharacter* GetFillainPlayerCharacter() const { return Character; }
	FORCEINLINE AFillainPlayerController* GetFillainPlayerController() const { return Controller; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);

};
