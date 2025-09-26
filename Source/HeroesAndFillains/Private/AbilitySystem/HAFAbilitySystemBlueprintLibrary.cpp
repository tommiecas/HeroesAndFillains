// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"

#include "Characters/CharacterClassInfo.h"
#include "GameMode/HaFGameMode.h"
#include "HUD/FillainHUD.h"
#include "HUD/WidgetControllers/HAFWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/HAFPlayerState.h"

UOverlayWidgetController* UHAFAbilitySystemBlueprintLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFillainHUD* FillainHUD = Cast<AFillainHUD>(PC->GetHUD()))
		{
			AHAFPlayerState* PS = PC->GetPlayerState<AHAFPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, AbilitySystemComponent, AS);
			return FillainHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UHAFAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFillainHUD* FillainHUD = Cast<AFillainHUD>(PC->GetHUD()))
		{
			AHAFPlayerState* PS = PC->GetPlayerState<AHAFPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, AbilitySystemComponent, AS);
			return FillainHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UHAFAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle InvisibleAttributesContextHandle = ASC->MakeEffectContext();
	InvisibleAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle InvisibleAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->InvisibleAttributes, Level, InvisibleAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*InvisibleAttributesSpecHandle.Data.Get());
}

void UHAFAbilitySystemBlueprintLibrary::GiveStartupAbilities(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject); 
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{	
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UHAFAbilitySystemBlueprintLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AHAFGameMode* HAFGameMode = Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (HAFGameMode == nullptr) return nullptr;
	return HAFGameMode->CharacterClassInfo;
}

FGameplayEffectContextHandle UHAFAbilitySystemBlueprintLibrary::AddSourceObjectToContext(
	const FGameplayEffectContextHandle& Context, UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	NewContext.AddSourceObject(SourceObject);
	return NewContext;
}

bool UHAFAbilitySystemBlueprintLibrary::IsGameplayEffectSpecHandleValid(const FGameplayEffectSpecHandle& SpecHandle)
{
	{
		return SpecHandle.IsValid();
	}
}

	
