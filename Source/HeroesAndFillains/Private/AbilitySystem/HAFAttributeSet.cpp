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
#include "Compression/lz4.h"
#include "Enemies/EnemyBase.h"
#include "HAFComponents/AttributeComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"





UHAFAttributeSet::UHAFAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);

	InitShield(75.f);
	InitMaxShield(100.f);

	InitStamina(20.f);
	InitMaxStamina(100.f);

	InitMajix(10.f);
	InitMaxMajix(100.f);
}

void UHAFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UHAFAttributeSet::SetAttributeFromComponent(FGameplayAttributeData& Attribute, float Value)
{
	const_cast<FGameplayAttributeData&>(Attribute).SetBaseValue(Value);
	const_cast<FGameplayAttributeData&>(Attribute).SetCurrentValue(Value);
}

void UHAFAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	if (Attribute == GetMaxHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	if (Attribute == GetShieldAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxShield.GetCurrentValue());
	if (Attribute == GetMaxShieldAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxShield.GetCurrentValue());
	if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	if (Attribute == GetMaxStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	if (Attribute == GetMajixAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxMajix.GetCurrentValue());
	if (Attribute == GetMaxMajixAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, MaxMajix.GetCurrentValue());
}

void UHAFAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)
	
	Properties.EffectContextHandle = Data.EffectSpec.GetContext();
	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Properties.SourceASC) && Properties.SourceASC->AbilityActorInfo.IsValid() && Properties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Properties.SourceAvatarActor = Properties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Properties.SourceController = Properties.SourceASC->AbilityActorInfo->PlayerController.Get();
		
		if (Properties.SourceController == nullptr && Properties.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Properties.SourceAvatarActor))
			{
				Properties.SourceController = Pawn->GetController();
			}
		}
		if (Properties.SourceController)
		{
			Properties.SourceCharacter = Cast<ACharacter>(Properties.SourceController->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Properties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Properties.TargetCharacter = Cast<ACharacter>(Properties.TargetAvatarActor);
		Properties.TargetASC = &Data.Target; 
	}
}

void UHAFAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Properties;
	SetEffectProperties(Data, Properties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetMajixAttribute())
	{
		SetMajix(FMath::Clamp(GetMajix(), 0.f, GetMaxMajix()));
	}
	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
}