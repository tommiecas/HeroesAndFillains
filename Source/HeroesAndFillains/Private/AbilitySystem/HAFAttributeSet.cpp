// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/FillainPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "Characters/FillainCharacter.h"
#include "Enemies/EnemyBase.h"
#include "HAFComponents/AttributeComponent.h"


UHAFAttributeSet::UHAFAttributeSet()
{
	InitHealth(100.f);
	InitShield(100.f);
	InitStamina(100.f);
	InitMajix(100.f);
	InitMaxHealth(100.f);
	InitMaxShield(100.f);
	InitMaxStamina(100.f);
	InitMaxMajix(100.f);
}

void UHAFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Majix, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxMajix, COND_None, REPNOTIFY_Always);

}

void UHAFAttributeSet::SetAttributeFromComponent(FGameplayAttributeData& Attribute, float Value)
{
	const_cast<FGameplayAttributeData&>(Attribute).SetBaseValue(Value);
	const_cast<FGameplayAttributeData&>(Attribute).SetCurrentValue(Value);
}

void UHAFAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Health, OldHealth);
}

void UHAFAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxHealth, OldMaxHealth);
}

void UHAFAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Shield, OldShield);
}

void UHAFAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxShield, OldMaxShield);
}

void UHAFAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Stamina, OldStamina);
}

void UHAFAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxStamina, OldMaxStamina);
}

void UHAFAttributeSet::OnRep_Majix(const FGameplayAttributeData& OldMajix) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Majix, OldMajix);
}

void UHAFAttributeSet::OnRep_MaxMajix(const FGameplayAttributeData& OldMaxMajix) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxMajix, OldMaxMajix);
}
