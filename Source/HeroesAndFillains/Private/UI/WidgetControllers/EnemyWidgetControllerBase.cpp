#include "UI/WidgetControllers/EnemyWidgetControllerBase.h"
#include "Enemies/EnemyBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/AttributeIhfo.h"
#include "AbilitySystem/HAFAttributeSet.h"

void UEnemyWidgetControllerBase::Initialize(AEnemyBase* InEnemy)
{
	if (!InEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyWidgetControllerBase::Initialize called with null Enemy."));
		return;
	}

	Enemy = InEnemy;
	OwningEnemy = InEnemy;

	// Try to get ASC and AttributeSet
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(InEnemy))
	{
		EnemyASC = ASCInterface->GetAbilitySystemComponent();
	}

	if (!EnemyASC)
	{
		EnemyASC = InEnemy->GetAbilitySystemComponent();
		if (!EnemyASC)
		{
			UE_LOG(LogTemp, Warning,
					TEXT("EnemyWidgetControllerBase: ASC is null for %s"),
					*GetNameSafe(GetOwningEnemy()));
			return;		}
	}

	EnemyHAFAttributeSet = Cast<UHAFAttributeSet>(EnemyASC->GetAttributeSet(UHAFAttributeSet::StaticClass()));
	if (!EnemyHAFAttributeSet)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("EnemyWidgetControllerBase: UHAFAttributeSet missing for %s"),
			*GetNameSafe(GetOwningEnemy()));
		return;
	}

	// ✅ Bind but don't broadcast yet
	AssignEnemyAttributeInfoDelegate();

	if (!InEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Initialize() failed — InEnemy is null"), *GetNameSafe(this));
		return;
	}
}

void UEnemyWidgetControllerBase::AssignEnemyAttributeInfoDelegate()
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: EnemyRef is null."), *GetNameSafe(this));
		return;
	}

	BindCallbacksToDependencies();

	UE_LOG(LogTemp, Log, TEXT("%s successfully assigned Enemy Attribute delegates."), *GetNameSafe(this));
}

void UEnemyWidgetControllerBase::BindCallbacksToDependencies()
{
	if (!EnemyASC || !OwningEnemy) return;

	if (!EnemyHAFAttributeSet)
	{
		EnemyHAFAttributeSet =
			Cast<UHAFAttributeSet>(EnemyASC->GetAttributeSet(UHAFAttributeSet::StaticClass()));
	}

	if (!EnemyHAFAttributeSet)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("EnemyWidgetControllerBase: UHAFAttributeSet missing for %s"),
			*GetNameSafe(GetOwningEnemy()));
		return;
	}

	if (OwningEnemy->EnemyAttributeInfoOverride)
	{
		EnemyAttributeInfo = OwningEnemy->EnemyAttributeInfoOverride;
		UE_LOG(LogTemp, Warning,
			TEXT("[EnemyWC] AttributeInfo overridden by %s’s data asset."),
			*OwningEnemy->GetName());
	}

	if (!EnemyAttributeInfo)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("EnemyWidgetControllerBase: EnemyAttributeInfo is null for %s"),
			*GetNameSafe(GetOwningEnemy()));
		return;
	}

	for (auto& Pair : EnemyHAFAttributeSet->TagsToAttributes)
	{
		EnemyASC->GetGameplayAttributeValueChangeDelegate(Pair.Value())
			.AddLambda([this, Pair](const FOnAttributeChangeData&)
			{
				BroadcastEnemyAttributeInfo(Pair.Key, Pair.Value());
			});
	}

	UE_LOG(LogTemp, Warning,
		TEXT("BindCallbacksToDependencies -> ASC: %s | AS: %s"),
		*GetNameSafe(EnemyASC),
		*GetNameSafe(EnemyHAFAttributeSet));
}

void UEnemyWidgetControllerBase::BroadcastInitialEnemyValues()
{
	// --- Required validity checks ---
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[%s] ❌ Enemy reference invalid!"),
			*GetNameSafe(this));
		return;
	}

	if (!EnemyASC)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[%s] ❌ EnemyASC is null!"),
			*GetNameSafe(this));
		return;
	}

	if (!EnemyHAFAttributeSet)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[%s] ❌ EnemyHAFAttributeSet is null!"),
			*GetNameSafe(this));
		return;
	}

	if (!EnemyAttributeInfo)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[%s] ❌ EnemyAttributeInfo is null!"),
			*GetNameSafe(this));
		return;
	}

	// --- Broadcast generic tag-based attribute info ---
	for (const auto& Pair : EnemyHAFAttributeSet->TagsToAttributes)
	{
		BroadcastEnemyAttributeInfo(Pair.Key, Pair.Value());
	}

	// --- Safety check before direct delegate broadcasts ---
	if (!IsValid(Enemy))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[%s] ❌ Enemy destroyed before broadcasting"),
			*GetNameSafe(this));
		return;
	}

	auto SafeBroadcast = [](auto& Delegate, float Value, const FString& Label)
	{
		if (Delegate.IsBound())
		{
			Delegate.Broadcast(Value);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Skipped broadcast: %s not bound"),
				*Label);
		}
	};

	// --- Explicit enemy stat broadcasts ---
	SafeBroadcast(Enemy->OnEnemyHealthChanged,     EnemyHAFAttributeSet->GetHealth(),     TEXT("Health"));
	SafeBroadcast(Enemy->OnEnemyMaxHealthChanged,  EnemyHAFAttributeSet->GetMaxHealth(),  TEXT("MaxHealth"));
	SafeBroadcast(Enemy->OnEnemyShieldChanged,     EnemyHAFAttributeSet->GetShield(),     TEXT("Shield"));
	SafeBroadcast(Enemy->OnEnemyMaxShieldChanged,  EnemyHAFAttributeSet->GetMaxShield(),  TEXT("MaxShield"));
	SafeBroadcast(Enemy->OnEnemyStaminaChanged,    EnemyHAFAttributeSet->GetStamina(),    TEXT("Stamina"));
	SafeBroadcast(Enemy->OnEnemyMaxStaminaChanged, EnemyHAFAttributeSet->GetMaxStamina(), TEXT("MaxStamina"));
	SafeBroadcast(Enemy->OnEnemyMajixChanged,      EnemyHAFAttributeSet->GetMajix(),      TEXT("Majix"));
	SafeBroadcast(Enemy->OnEnemyMaxMajixChanged,   EnemyHAFAttributeSet->GetMaxMajix(),   TEXT("MaxMajix"));

	UE_LOG(LogTemp, Warning,
		TEXT("[%s] ✅ BroadcastInitialEnemyValues() succeeded."),
		*GetNameSafe(this));
}


void UEnemyWidgetControllerBase::BroadcastEnemyAttributeInfo(const FGameplayTag& EnemyAttributeTag,
	const FGameplayAttribute& EnemyAttribute) const
{
	FHAFAttributeInfo EnemyInfo = EnemyAttributeInfo->FindAttributeInfoForTag(EnemyAttributeTag);
	EnemyInfo.AttributeValue = EnemyAttribute.GetNumericValue(EnemyHAFAttributeSet);
	EnemyAttributeInfoDelegate.Broadcast(EnemyInfo);

	
}
