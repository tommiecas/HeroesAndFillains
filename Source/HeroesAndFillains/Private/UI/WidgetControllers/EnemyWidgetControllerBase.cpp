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
			UE_LOG(LogTemp, Warning, TEXT("%s: Enemy has no ASC!"), *GetNameSafe(InEnemy));
		}
	}

	EnemyAttributeSet = InEnemy->GetAttributeSet();

	// ✅ Bind but don't broadcast yet
	AssignEnemyAttributeInfoDelegate();
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
	if (!EnemyASC) return;

	if (!EnemyAttributeSet)
	{
		UAbilitySystemComponent* EnAbSyCo = Enemy->GetEnemyASC();
		if (EnAbSyCo)
		{
			EnemyAttributeSet = EnAbSyCo->GetSet<UAttributeSet>();
		}
	}

	if (OwningEnemy && OwningEnemy->EnemyAttributeInfoOverride)
	{
		EnemyAttributeInfo = OwningEnemy->EnemyAttributeInfoOverride;
		// UE_LOG(LogTemp, Warning, TEXT("[EnemyWC] AttributeInfo overridden by %s’s data asset."), *OwningEnemy->GetName());
	}

	const UHAFAttributeSet* AS = CastChecked<UHAFAttributeSet>(EnemyAttributeSet);
	check(EnemyAttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		EnemyASC->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastEnemyAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	// UE_LOG(LogTemp, Warning, TEXT("BindCallbacksToDependencies -> ASC: %s | AS: %s"),
	// 	*GetNameSafe(EnemyASC), *GetNameSafe(EnemyAttributeSet));
}

void UEnemyWidgetControllerBase::BroadcastInitialEnemyValues()
{
	if (!EnemyAttributeInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: EnemyAttributeInfo is null! Likely using base class instead of BP subclass."),
			*GetNameSafe(this));
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("ControllerClass = %s | EnemyAttributeInfo = %s"),
	// 	*GetClass()->GetName(),
	// 	*GetNameSafe(EnemyAttributeInfo));

	EnemyAttributeSet = Enemy->GetAttributeSet();
	if (!EnemyAttributeSet)
	{
		UAbilitySystemComponent* EnAbSyCo = Enemy->GetEnemyASC();
		if (EnAbSyCo)
		{
			EnemyAttributeSet = EnAbSyCo->GetSet<UAttributeSet>();
		}
	}

	if (OwningEnemy && OwningEnemy->EnemyAttributeInfoOverride)
	{
		EnemyAttributeInfo = OwningEnemy->EnemyAttributeInfoOverride;
		// UE_LOG(LogTemp, Warning, TEXT("[EnemyWC] AttributeInfo overridden by %s’s data asset."), *OwningEnemy->GetName());
	}

	const UHAFAttributeSet* EnemyAS = CastChecked<UHAFAttributeSet>(EnemyAttributeSet);
	check(EnemyAttributeInfo);

	// ✅ Broadcast generic tag-based info for Attribute Menus
	for (auto& Pair : EnemyAS->TagsToAttributes)
	{
		BroadcastEnemyAttributeInfo(Pair.Key, Pair.Value());
	}

	// ✅ Also broadcast typed stat values for health/shield bar widgets
	OwningEnemy->OnEnemyHealthChanged.Broadcast(EnemyAS->GetHealth());
	OwningEnemy->OnEnemyMaxHealthChanged.Broadcast(EnemyAS->GetMaxHealth());
	OwningEnemy->OnEnemyShieldChanged.Broadcast(EnemyAS->GetShield());
	OwningEnemy->OnEnemyMaxShieldChanged.Broadcast(EnemyAS->GetMaxShield());
	OwningEnemy->OnEnemyStaminaChanged.Broadcast(EnemyAS->GetStamina());
	OwningEnemy->OnEnemyMaxStaminaChanged.Broadcast(EnemyAS->GetMaxStamina());
	OwningEnemy->OnEnemyMajixChanged.Broadcast(EnemyAS->GetMajix());
	OwningEnemy->OnEnemyMaxMajixChanged.Broadcast(EnemyAS->GetMaxMajix());

	/* UE_LOG(LogTemp, Warning, TEXT("[%s] BroadcastInitialEnemyValues() -> "
		"H=%f/%f | S=%f/%f | Stamina=%f/%f | Majix=%f/%f"),
		*GetNameSafe(this),
		EnemyAS->GetHealth(), EnemyAS->GetMaxHealth(),
		EnemyAS->GetShield(), EnemyAS->GetMaxShield(),
		EnemyAS->GetStamina(), EnemyAS->GetMaxStamina(),
		EnemyAS->GetMajix(), EnemyAS->GetMaxMajix());*/
}

void UEnemyWidgetControllerBase::BroadcastEnemyAttributeInfo(const FGameplayTag& EnemyAttributeTag,
	const FGameplayAttribute& EnemyAttribute) const
{
	FHAFAttributeInfo EnemyInfo = EnemyAttributeInfo->FindAttributeInfoForTag(EnemyAttributeTag);
	EnemyInfo.AttributeValue = EnemyAttribute.GetNumericValue(EnemyAttributeSet);
	EnemyAttributeInfoDelegate.Broadcast(EnemyInfo);

	
}
