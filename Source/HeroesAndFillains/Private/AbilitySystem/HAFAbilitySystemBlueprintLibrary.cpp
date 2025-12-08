// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"

#include "HAFAbilityTypes.h"
#include "Characters/CharacterClassInfo.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/EnemyInfo.h"
#include "Engine/OverlapResult.h"
#include "Engine/SceneCapture2D.h"
#include "GameMode/HaFGameMode.h"
#include "UI/FillainHUD.h"
#include "UI/WidgetControllers/AttributeMenuWidgetController.h"
#include "UI/WidgetControllers/HAFWidgetController.h"
#include "UI/Widgets/EnemyAttributeMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/HAFPlayerState.h"
#include "UI/WidgetControllers/HAFWidgetController.h"

bool UHAFAbilitySystemBlueprintLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AFillainHUD*& OutFillainHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutFillainHUD = Cast<AFillainHUD>(PC->GetHUD());
		if (OutFillainHUD)
		{
			AHAFPlayerState* PS = PC->GetPlayerState<AHAFPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}


UOverlayWidgetController* UHAFAbilitySystemBlueprintLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AFillainHUD* FillainHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, FillainHUD))
	{
		return FillainHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UHAFAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(
		const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AFillainHUD* FillainHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, FillainHUD))
	{
		return FillainHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UHAFAbilitySystemBlueprintLibrary::GetSpellMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AFillainHUD* FillainHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, FillainHUD))
	{
		return FillainHUD->GetSpellMenuWidgetController(WCParams);
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
	UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject); 
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{	
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.ClassAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			if (ABaseCharacter* AvatarActorAsBase = Cast<ABaseCharacter>(ASC->GetAvatarActor()))
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor(), AvatarActorAsBase));		
				ASC->GiveAbility(AbilitySpec);
			}
		}
	}
}

UCharacterClassInfo* UHAFAbilitySystemBlueprintLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AHAFGameMode* HAFGameMode = Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (HAFGameMode == nullptr) return nullptr;
	return HAFGameMode->CharacterClassInfo;
}

UEnemyInfo* UHAFAbilitySystemBlueprintLibrary::GetEnemyInfo(const UObject* WorldContextObject)
{
	const AHAFGameMode* HAFGameMode = Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (HAFGameMode == nullptr) return nullptr;
	return HAFGameMode->EnemyInfo;
}

UAbilityInfo* UHAFAbilitySystemBlueprintLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AHAFGameMode* HAFGameMode = Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (HAFGameMode == nullptr) return nullptr;
	return HAFGameMode->AbilityInfo;
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

bool UHAFAbilitySystemBlueprintLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UHAFAbilitySystemBlueprintLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UHAFAbilitySystemBlueprintLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsBlockedHit)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UHAFAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const TArray<FVector>& SphereOrigins)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	// Loop through each origin and do a separate overlap check
	for (const FVector& Origin : SphereOrigins)
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(
			Overlaps, 
			Origin,  // Single origin point
			FQuat::Identity, 
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), 
			FCollisionShape::MakeSphere(Radius), 
			SphereParams
		);

		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UHAFAbilitySystemBlueprintLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFriends = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player")) ||
			FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !bFriends;
}

int32 UHAFAbilitySystemBlueprintLibrary::GetXPAwardForEnemyTypeAndLevel(const UObject* WorldContextObject,
	EEnemyType EnemyType, int32 EnemyLevel)
{
	UEnemyInfo* EnemyInfo = GetEnemyInfo(WorldContextObject);
	if (EnemyInfo == nullptr) return 0;

	const FEnemyWiki& EnemyWiki = EnemyInfo->GetEnemyInfo(EnemyType);
	const float XPAward = EnemyWiki.XPReward.GetValueAtLevel(EnemyLevel);

	return static_cast<int32>(XPAward);
}

	
