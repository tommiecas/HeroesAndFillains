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

	if (!InEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Initialize() failed — InEnemy is null"), *GetNameSafe(this));
		return;
	}

	CachedASC = InEnemy->GetAbilitySystemComponent();
	CachedAttributeSet = InEnemy->GetAttributeSet();

	if (!CachedASC || !CachedAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Enemy missing ASC or AttributeSet — skipping widget setup"), *GetNameSafe(this));
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
	if (CachedASC.Get() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ CachedASC invalid during BroadcastInitialEnemyValues"), *GetNameSafe(this));
		return;
	}
	if (CachedAttributeSet.Get() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ CachedAttributeSet invalid during BroadcastInitialEnemyValues"), *GetNameSafe(this));
		return;
	}
	if (EnemyAttributeInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ EnemyAttributeInfo is null!"), *GetNameSafe(this));
		return;
	}
	if (Enemy == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ Enemy reference invalid!"), *GetNameSafe(this));
		return;
	}

	const UHAFAttributeSet* EnemyAS = Cast<UHAFAttributeSet>(CachedAttributeSet.Get());
	if (EnemyAS == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ CachedAttributeSet is not UHAFAttributeSet!"), *GetNameSafe(this));
		return;
	}

	// ✅ Broadcast generic tag-based info
	for (const auto& Pair : EnemyAS->TagsToAttributes)
	{
		BroadcastEnemyAttributeInfo(Pair.Key, Pair.Value());
	}

	if (!IsValid(Enemy))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ Enemy destroyed before broadcasting"), *GetNameSafe(this));
		return;
	}
	
	auto SafeBroadcast = [](auto& Delegate, auto Value, const FString& Label)
	{
		if (Delegate.IsBound())
		{
			Delegate.Broadcast(Value);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skipped broadcast: %s not bound"), *Label);
		}
	};

	SafeBroadcast(Enemy->OnEnemyHealthChanged, EnemyAS->GetHealth(), TEXT("Health"));
	SafeBroadcast(Enemy->OnEnemyMaxHealthChanged, EnemyAS->GetMaxHealth(), TEXT("MaxHealth"));
	SafeBroadcast(Enemy->OnEnemyShieldChanged, EnemyAS->GetShield(), TEXT("Shield"));
	SafeBroadcast(Enemy->OnEnemyMaxShieldChanged, EnemyAS->GetMaxShield(), TEXT("MaxShield"));
	SafeBroadcast(Enemy->OnEnemyStaminaChanged, EnemyAS->GetStamina(), TEXT("Stamina"));
	SafeBroadcast(Enemy->OnEnemyMaxStaminaChanged, EnemyAS->GetMaxStamina(), TEXT("MaxStamina"));
	SafeBroadcast(Enemy->OnEnemyMajixChanged, EnemyAS->GetMajix(), TEXT("Majix"));
	SafeBroadcast(Enemy->OnEnemyMaxMajixChanged, EnemyAS->GetMaxMajix(), TEXT("MaxMajix"));


	UE_LOG(LogTemp, Warning, TEXT("[%s] ✅ BroadcastInitialEnemyValues() succeeded."), *GetNameSafe(this));
}


void UEnemyWidgetControllerBase::BroadcastEnemyAttributeInfo(const FGameplayTag& EnemyAttributeTag,
	const FGameplayAttribute& EnemyAttribute) const
{
	FHAFAttributeInfo EnemyInfo = EnemyAttributeInfo->FindAttributeInfoForTag(EnemyAttributeTag);
	EnemyInfo.AttributeValue = EnemyAttribute.GetNumericValue(EnemyAttributeSet);
	EnemyAttributeInfoDelegate.Broadcast(EnemyInfo);

	
}
