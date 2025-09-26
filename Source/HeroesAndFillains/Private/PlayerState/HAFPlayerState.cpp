// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/HAFPlayerState.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/HAFAttributeSet.h"   // <- concrete set (derives from UAttributeSet)
#include "AbilitySystemComponent.h"

AHAFPlayerState::AHAFPlayerState()
{
	HAFAbilitySystemComponent = CreateDefaultSubobject<UHAFAbilitySystemComponent>(TEXT("ASC"));
	HAFAbilitySystemComponent->SetIsReplicated(true);
	HAFAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HAFAttributeSet = CreateDefaultSubobject<UHAFAttributeSet>(TEXT("HAFAttributeSet"));
	HAFAbilitySystemComponent->AddAttributeSetSubobject(HAFAttributeSet.Get());
	
	// (optional) quick sanity logs
	UE_LOG(LogTemp, Warning, TEXT("[PS::Ctor] ASC=%s AS=%s"),
		*GetNameSafe(HAFAbilitySystemComponent), *GetNameSafe(HAFAttributeSet));

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
	return HAFAbilitySystemComponent;
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

	/*if (AFillainCharacter* BCharacter = Cast<AFillainCharacter>(GetPawn()))
	{
		BCharacter->SetTeamColor(Team);
	}*/
}

void AHAFPlayerState::OnRep_Team()
{
	/*if (AFillainCharacter* BCharacter = Cast<AFillainCharacter>(GetPawn()))
	{
		BCharacter->SetTeamColor(Team);
	}*/
}