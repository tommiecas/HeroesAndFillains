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
#include "AbilitySystem/AbilityInfo.h"
#include "Engine/DataTable.h"          // UDataTable
#include "Items/CustomDesignedPCPickupItem.h"            // ACustomDesignedPCPickupItem (for SourceObject cast)

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
	
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(HAFAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(HAFAttributeSet->GetMaxHealth());
	OnShieldChanged.Broadcast(HAFAttributeSet->GetShield());
	OnMaxShieldChanged.Broadcast(HAFAttributeSet->GetMaxShield());
	OnStaminaChanged.Broadcast(HAFAttributeSet->GetStamina());
	OnMaxStaminaChanged.Broadcast(HAFAttributeSet->GetMaxStamina());
	OnMajixChanged.Broadcast(HAFAttributeSet->GetMajix());
	OnMaxMajixChanged.Broadcast(HAFAttributeSet->GetMaxMajix());

	UE_LOG(LogTemp, Warning, TEXT("BroadcastInitialValues ASC=%s AS=%s Controller=%s"),
	*GetNameSafe(AbilitySystemComponent),
	*GetNameSafe(AttributeSet),
	*GetNameSafe(this));

	UE_LOG(LogTemp, Warning, TEXT("Health init = %f / %f"),
	HAFAttributeSet->GetHealth(), HAFAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[OverlayWidgetController] Missing ASC or AS when binding!"));
		return;
	}
	const UHAFAttributeSet* HAFAttributeSet = CastChecked<UHAFAttributeSet>(AttributeSet);

	UE_LOG(LogTemp, Warning, TEXT("[OverlayWidgetController] Successfully binding attribute delegates for ASC=%s, AS=%s"),
		  *GetNameSafe(AbilitySystemComponent), *GetNameSafe(AttributeSet));
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnHealthChanged"));

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxHealthChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnShieldChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnShieldChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxShieldChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxShieldChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetStaminaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnStaminaChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnStaminaChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxStaminaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxStaminaChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxStaminaChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMajixAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMajixChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMajixChanged"));


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		HAFAttributeSet->GetMaxMajixAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxMajixChanged.Broadcast(Data.NewValue);
			});
	// UE_LOG(LogTemp, Warning, TEXT("[WidgetController] Bound OnMaxMajixChanged"));

	// AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf
	// .AddUObject(this, &UOverlayWidgetController::OnGEAddedToSelf);
	
	// Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	// 	[this] (const FGameplayTagContainer& AssetTags)
	if (UHAFAbilitySystemComponent* HAFASC = Cast<UHAFAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (HAFASC->bStartupAbilitiesGiven)
		{
			OnInitializeStartupAbilities(HAFASC);
		}
		else
		{
			HAFASC->AbilitiesGiven.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}

		HAFASC->EffectAssetTags.AddLambda(
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
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetHealth();
}

float UOverlayWidgetController::GetMaxHealth() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetMaxHealth();
}

float UOverlayWidgetController::GetCurrentShield() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetShield();
}

float UOverlayWidgetController::GetMaxShield() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetMaxShield();
}

float UOverlayWidgetController::GetCurrentStamina() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetStamina();
}

float UOverlayWidgetController::GetMaxStamina() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetMaxStamina();
}

float UOverlayWidgetController::GetCurrentMajix() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetMajix();
}

float UOverlayWidgetController::GetMaxMajix() const
{
	if (!AttributeSet) return 0.f;
	UHAFAttributeSet* HAFAttributeSet = Cast<UHAFAttributeSet>(AttributeSet);
	return HAFAttributeSet->GetMaxMajix();
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UHAFAbilitySystemComponent* HAFAbilitySystemComponent)
{
	//TODO: Get information about all given abilities, look up their Ability Tag, and broadcast to all widgets.
	if (!HAFAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, HAFAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		//TODO: Need a way to figure out the ability tag for a given ability spec
		FHAFAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(HAFAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = HAFAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	HAFAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
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

