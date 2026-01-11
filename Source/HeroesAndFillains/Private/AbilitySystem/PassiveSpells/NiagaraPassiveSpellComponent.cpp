// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PassiveSpells/NiagaraPassiveSpellComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "Interfaces/CombatInterface.h"

UNiagaraPassiveSpellComponent::UNiagaraPassiveSpellComponent()
{
	bAutoActivate = false;
}

void UNiagaraPassiveSpellComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UHAFAbilitySystemComponent* HAFASC = Cast<UHAFAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		HAFASC->ActivatePassiveEffect.AddUObject(this, &UNiagaraPassiveSpellComponent::OnPassiveActivate);
		ActivateIfEquipped(HAFASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UHAFAbilitySystemComponent* HAFASC = Cast<UHAFAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
			{
				HAFASC->ActivatePassiveEffect.AddUObject(this, &UNiagaraPassiveSpellComponent::OnPassiveActivate);
				ActivateIfEquipped(HAFASC);
			}
		});
	}
}

void UNiagaraPassiveSpellComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UNiagaraPassiveSpellComponent::ActivateIfEquipped(UHAFAbilitySystemComponent* HAFASC)
{
	const bool bStartupAbilitiesGiven = HAFASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (HAFASC->GetStatusFromAbilityTag(PassiveSpellTag) == FHAFGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}
