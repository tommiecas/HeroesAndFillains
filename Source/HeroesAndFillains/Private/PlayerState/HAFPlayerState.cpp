// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/HAFPlayerState.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"

AHAFPlayerState::AHAFPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	check(AbilitySystemComponent);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UHAFAttributeSet>(TEXT("AttributeSet"));
	check(AttributeSet);

	SetNetUpdateFrequency(100.f);
}

void AHAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHAFPlayerState, Defeats);
	DOREPLIFETIME(AHAFPlayerState, Team);
	DOREPLIFETIME(AHAFPlayerState, Level);
}

void AHAFPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AFillainCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AHAFPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AFillainCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

UAbilitySystemComponent* AHAFPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AHAFPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;

	Character = Character == nullptr ? Cast<AFillainCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AHAFPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AFillainCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFillainPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AHAFPlayerState::OnRep_Level(int32 OldLevel)
{
	
}

void AHAFPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	if (AFillainCharacter* BCharacter = Cast<AFillainCharacter>(GetPawn()))
	{
		BCharacter->SetTeamColor(Team);
	}
}

void AHAFPlayerState::OnRep_Team()
{
	if (AFillainCharacter* BCharacter = Cast<AFillainCharacter>(GetPawn()))
	{
		BCharacter->SetTeamColor(Team);
	}
}