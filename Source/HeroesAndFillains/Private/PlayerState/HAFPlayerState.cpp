// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/HAFPlayerState.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/FillainCharacter.h"
#include "PlayerController/FillainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/HAFAttributeSet.h"   // <- concrete set (derives from UAttributeSet)
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HAFGameplayAbility.h"

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
	DOREPLIFETIME(AHAFPlayerState, XP);
	DOREPLIFETIME(AHAFPlayerState, AttributePoints);
	DOREPLIFETIME(AHAFPlayerState, SpellPoints);

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

void AHAFPlayerState::AddLevel(const int32 LevelToAdd)
{
	Level += LevelToAdd;
	OnLevelChangedDelegate.Broadcast(Level);

	// Update ability costs
	UpdateAbilityCosts(Level);
}

void AHAFPlayerState::SetPlayerLevel(const int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChangedDelegate.Broadcast(Level);

	// Update ability costs
	UpdateAbilityCosts(Level);
}

void AHAFPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AHAFPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AHAFPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AHAFPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AHAFPlayerState::AddXP(const int32 XPToAdd)
{
	XP += XPToAdd;
	OnXPChangedDelegate.Broadcast(XP);
}

void AHAFPlayerState::SetPlayerXP(const int32 NewXP)
{
	XP = NewXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AHAFPlayerState::AddAttributePoints(const int32 AttributePointsToAdd)
{
	AttributePoints += AttributePointsToAdd;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AHAFPlayerState::SetAttributePoints(const int32 NewAttributePoints)
{
	AttributePoints = NewAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AHAFPlayerState::AddSpellPoints(const int32 SpellPointsToAdd)
{
	SpellPoints += SpellPointsToAdd;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AHAFPlayerState::SetSpellPoints(const int32 NewSpellPoints)
{
	SpellPoints = NewSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AHAFPlayerState::UpdateAbilityCosts(int32 PlayerCharacterLevel)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	// Loop all granted abilities
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			UHAFGameplayAbility* Ability = Cast<UHAFGameplayAbility>(Spec.Ability);
			if (Ability)
			{
				float NewCost = Ability->CalculateAbilityCost(PlayerCharacterLevel);

				// Store it however you want
				NewCost = Ability->AbilityCost;

				// Or update a GAS SetByCaller, etc
			}
		}
	}
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