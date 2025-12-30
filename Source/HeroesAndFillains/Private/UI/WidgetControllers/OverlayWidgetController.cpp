// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/OverlayWidgetController.h"

#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "UI/Widgets/HAFUserWidget.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetControllers/OverlayWidgetController.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"            // FGameplayEffectSpec, UGameplayEffect
#include "GameplayEffectTypes.h"       // FActiveGameplayEffectHandle
#include "HAFGameplayTags.h"
#include "AbilitySystem/AbilityInfo.h"
#include "AbilitySystem/LevelUpInfo.h"
#include "Engine/DataTable.h"          // UDataTable
#include "HeroesAndFillains/HAFLogChannels.h"
#include "Items/CustomDesignedPCPickupItem.h"            // ACustomDesignedPCPickupItem (for SourceObject cast)
#include "PlayerState/HAFPlayerState.h"
#include "HeroesAndFillains/HAFLogChannels.h"


void UOverlayWidgetController::OnGEAddedToSelf(UAbilitySystemComponent* /*TargetASC*/,
                                               const FGameplayEffectSpec& SpecApplied,
                                               FActiveGameplayEffectHandle /*ActiveHandle*/)
{
		const UObject* SourceObj = SpecApplied.GetContext().GetSourceObject();
		const ACustomDesignedPCPickupItem* EffectActor = Cast<ACustomDesignedPCPickupItem>(SourceObj);
		LastEffectActorLevel = EffectActor ? EffectActor->GetActorLevel() : 0;

		const FGameplayTagContainer* AssetTags =
			SpecApplied.Def ? &SpecApplied.Def->GetAssetTags() : nullptr;
		if (!AssetTags) return;

		const FGameplayTag MessageRoot = FGameplayTag::RequestGameplayTag(TEXT("Message"));

		FGameplayTag RowTag;
		for (const FGameplayTag& Tag : *AssetTags)
		{
			if (Tag.MatchesTag(MessageRoot))
			{
				RowTag = Tag;
				break;
			}
		}
		if (!RowTag.IsValid()) return;

		// Remember last tag for replay
		LastRowTag = RowTag;

		if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, RowTag))
		{
			// Optional guard: only broadcast if the row has a widget class set
			if (Row->MessageWidget) 
			{
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	}

	// NEW: simple replay that only fires if we have a valid tag and a valid row + widget class
	void UOverlayWidgetController::ReplayLastMessage()
	{
		if (!LastRowTag.IsValid() || !MessageWidgetDataTable) return;

		if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, LastRowTag))
		{
			if (Row->MessageWidget)
			{
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	}
	
void UOverlayWidgetController::BroadcastInitialValues()
{
	
	OnHealthChanged.Broadcast(GetHAFAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetHAFAttributeSet()->GetMaxHealth());
	OnShieldChanged.Broadcast(GetHAFAttributeSet()->GetShield());
	OnMaxShieldChanged.Broadcast(GetHAFAttributeSet()->GetMaxShield());
	OnStaminaChanged.Broadcast(GetHAFAttributeSet()->GetStamina());
	OnMaxStaminaChanged.Broadcast(GetHAFAttributeSet()->GetMaxStamina());
	OnMajixChanged.Broadcast(GetHAFAttributeSet()->GetMajix());
	OnMaxMajixChanged.Broadcast(GetHAFAttributeSet()->GetMaxMajix());

	UE_LOG(LogTemp, Warning, TEXT("BroadcastInitialValues ASC=%s AS=%s Controller=%s"),
	*GetNameSafe(AbilitySystemComponent),
	*GetNameSafe(AttributeSet),
	*GetNameSafe(this));

	UE_LOG(LogTemp, Warning, TEXT("Health init = %f / %f"),
	HAFAttributeSet->GetHealth(), HAFAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent || !AttributeSet || !PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[OverlayWidgetController] Missing ASC or AS or PS when binding!"));
		return;
	}
	GetHAFPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetHAFPlayerState()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);	

	UE_LOG(LogTemp, Warning, TEXT("[OverlayWidgetController] Successfully binding attribute delegates for ASC=%s, AS=%s, PS=%s"), *GetNameSafe(AbilitySystemComponent), *GetNameSafe(AttributeSet), *GetNameSafe(PlayerState));
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnHealthChanged"));

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxHealthChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnShieldChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnShieldChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetMaxShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxShieldChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxShieldChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetStaminaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnStaminaChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnStaminaChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetMaxStaminaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxStaminaChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxStaminaChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetMajixAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMajixChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMajixChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetHAFAttributeSet()->GetMaxMajixAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxMajixChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxMajixChanged"));

	// AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf
	// .AddUObject(this, &UOverlayWidgetController::OnGEAddedToSelf);
	
	// Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	// 	[this] (const FGameplayTagContainer& AssetTags)

	if (GetHAFAbilitySystemComponent())
	{
		GetHAFAbilitySystemComponent()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
	}

	if (GetHAFAbilitySystemComponent())
	{
		if (GetHAFAbilitySystemComponent()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetHAFAbilitySystemComponent()->AbilitiesGiven.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		GetHAFAbilitySystemComponent()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
			);
	}
}
				
	
	// UE_LOG(LogTemp, Warning, TEXT("[WC] Bound AddedDelegate on ASC=%s"),
		//   *GetNameSafe(AbilitySystemComponent));

float UOverlayWidgetController::GetCurrentHealth() const
{
	if (!AttributeSet) return 0.f;
	return HAFAttributeSet->GetHealth();
}

float UOverlayWidgetController::GetMaxHealth()
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetMaxHealth();
}

float UOverlayWidgetController::GetCurrentShield() 
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetShield();
}

float UOverlayWidgetController::GetMaxShield()
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetMaxShield();
}

float UOverlayWidgetController::GetCurrentStamina() 
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetStamina();
}

float UOverlayWidgetController::GetMaxStamina() 
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetMaxStamina();
}

float UOverlayWidgetController::GetCurrentMajix()
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetMajix();
}

float UOverlayWidgetController::GetMaxMajix() 
{
	if (!AttributeSet) return 0.f;
	return GetHAFAttributeSet()->GetMaxMajix();
}

 
void UOverlayWidgetController::BroadcastAllAbilityInfo()
{
	if (!AbilitySystemComponent) return;

	if (UHAFAbilitySystemComponent* HAFASC = Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent))
	{
		FForEachAbility BroadcastDelegate;
		BroadcastDelegate.BindLambda([this, HAFASC](const FGameplayAbilitySpec& Spec)
		{
			FHAFAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(HAFASC->GetAbilityTagFromSpec(Spec));
			Info.InputTag = HAFASC->GetInputTagFromSpec(Spec);
			AbilityInfoDelegate.Broadcast(Info);
		});

		HAFASC->ForEachAbility(BroadcastDelegate);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlotTag)
{
	const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();

	FHAFAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlotTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FHAFAbilityInfo CurrentInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	CurrentInfo.StatusTag = StatusTag;
	CurrentInfo.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(CurrentInfo);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) 
{
	const ULevelUpInfo* LevelUpInfo = GetHAFPlayerState()->LevelUpInformation;

	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo, Please fill out HAFPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level >= 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level -1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

