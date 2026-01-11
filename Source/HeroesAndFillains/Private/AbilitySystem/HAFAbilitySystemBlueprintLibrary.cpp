// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "HAFAbilityTypes.h"
#include "HAFGameplayTags.h"
#include "Characters/CharacterClassInfo.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/EnemyInfo.h"
#include "Engine/OverlapResult.h"
#include "Engine/SceneCapture2D.h"
#include "GameMode/HaFGameMode.h"
#include "GameMode/HAFSaveGame.h"
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

void UHAFAbilitySystemBlueprintLibrary::LoadAndInitializeAttributesFromSaveData(const UObject* WorldContextObject, ECharacterClass CharacterClass, 
	UAbilitySystemComponent* ASC, UHAFSaveGame* SavedGame)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);


	const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();
	const AActor* SourceAvatarActor = ASC->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_LoadedAndSetByCaller, 1.f, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Strength, SavedGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Intelligence, SavedGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Resilience, SavedGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Vigor, SavedGame->Vigor);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Dexterity, SavedGame->Dexterity);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Marksmanship, SavedGame->Marksmanship);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Wisdom, SavedGame->Wisdom);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Primary_Charisma, SavedGame->Charisma);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(SourceAvatarActor); 
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes_Infinite, 1.f, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.VitalAttributes, 1.f, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle InvisibleAttributesContextHandle = ASC->MakeEffectContext();
	InvisibleAttributesContextHandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle InvisibleAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->InvisibleAttributes, 1.f, InvisibleAttributesContextHandle);
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

bool UHAFAbilitySystemBlueprintLibrary::IsDebuffSuccessful(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->IsDebuffSuccessful();
	}
	return false;
}

float UHAFAbilitySystemBlueprintLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UHAFAbilitySystemBlueprintLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UHAFAbilitySystemBlueprintLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UHAFAbilitySystemBlueprintLibrary::GetDamageTypeTag(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (HAFGameplayEffectContext->GetDamageTypeTag().IsValid())
		{
			return *HAFGameplayEffectContext->GetDamageTypeTag();
		}
	}
	return FGameplayTag();	
}

FVector UHAFAbilitySystemBlueprintLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;	
}

FVector UHAFAbilitySystemBlueprintLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;		
}

bool UHAFAbilitySystemBlueprintLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->IsRadialDamage();
	}
	return false;
}

float UHAFAbilitySystemBlueprintLibrary::GetRadialDamageInnerRadius(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UHAFAbilitySystemBlueprintLibrary::GetRadialDamageOuterRadius(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UHAFAbilitySystemBlueprintLibrary::GetRadialDamageOrigin(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<const FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return HAFGameplayEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;	
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

void UHAFAbilitySystemBlueprintLibrary::SetIsDebuffSuccessful(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsDebuffSuccessful)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetIsDebuffSuccessful(bInIsDebuffSuccessful);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float Damage)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetDebuffDamage(Damage);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,
	float Duration)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetDebuffDuration(Duration);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
	float Frequency)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetDebuffFrequency(Frequency);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetDamageTypeTag(FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageTypeTag)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageTypeTagPtr = MakeShared<FGameplayTag>(InDamageTypeTag);
		HAFGameplayEffectContext->SetDamageTypeTag(DamageTypeTagPtr);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector InImpulse)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector InForce)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetKnockbackForce(InForce);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsRadialDamage)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InInnerRadius)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InOuterRadius)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InOrigin)
{
	if (FHAFGameplayEffectContext* HAFGameplayEffectContext = static_cast<FHAFGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		HAFGameplayEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UHAFAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(
			Overlaps, 
			SphereOrigin,  // Single origin point
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

void UHAFAbilitySystemBlueprintLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;
	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0) break;
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UHAFAbilitySystemBlueprintLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bFriends = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player")) ||
			FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !bFriends;
}

FGameplayEffectContextHandle UHAFAbilitySystemBlueprintLibrary::ApplyDamageEffect(
	const FDamageEffectParams& DamageEffectParams)
{
	const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageTypeTag, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	if (DamageEffectParams.TargetAbilitySystemComponent)
	{
		DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
	return EffectContextHandle;
}

TArray<FRotator> UHAFAbilitySystemBlueprintLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UHAFAbilitySystemBlueprintLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
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

void UHAFAbilitySystemBlueprintLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams,
	bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UHAFAbilitySystemBlueprintLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams,
	FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;		
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}

}

void UHAFAbilitySystemBlueprintLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams,
	FVector DeathImpulseDirection, float Magnitude)
{
	DeathImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse == DeathImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse == DeathImpulseDirection * Magnitude;
	}
}

void UHAFAbilitySystemBlueprintLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

FString UHAFAbilitySystemBlueprintLibrary::GetCurrentMapName(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return FString();

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World) return FString();

	FString CurrentMapName = World->GetMapName();

	// Strip PIE prefix
	CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);

	return CurrentMapName;
}

	
