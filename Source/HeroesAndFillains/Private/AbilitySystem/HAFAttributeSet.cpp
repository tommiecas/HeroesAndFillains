#include "AbilitySystem/HAFAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "Enemies/EnemyBase.h"
#include "HAFComponents/CombatComponent.h"
#include "UI/Widgets/EnemyProgressBarBaseWidget.h"
#include "UI/Widgets/EnemyStatsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/FillainPlayerController.h"
#include "HAFGameplayTags.h"

UHAFAttributeSet::UHAFAttributeSet() 
{
	const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();

	// Primary
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Dexterity, GetDexterityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Marksmanship, GetMarksmanshipAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Wisdom, GetWisdomAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Charisma, GetCharismaAttribute);

	// Secondary
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Agility, GetAgilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Flexibility, GetFlexibilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Purity, GetPurityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Corruptibility, GetCorruptibilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Intuition, GetIntuitionAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Vision, GetVisionAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Speed, GetSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Charm, GetCharmAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ShieldRegeneration, GetShieldRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_StaminaRegeneration, GetStaminaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MajixRegeneration, GetMajixRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxShield, GetMaxShieldAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxStamina, GetMaxStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMajix, GetMaxMajixAttribute);

	// Resistances
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireproofAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Ice, GetThermalRadiationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetShockproofAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_MeleeAttacks, GetInvulnerabilityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_RuleOfOrder, GetHeartOfDarknessAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_ChaosMajix, GetChaosIncorruptibleAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Toxicity, GetImmunityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Stun, GetUnstoppableAttribute);
	
	// Vital
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health,GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Shield,GetShieldAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Stamina,GetStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Majix,GetMajixAttribute);

	//Damage
	TagsToAttributes.Add(GameplayTags.Damage_IncomingDamage, GetIncomingDamageAttribute);
	
    // Invisible
    TagsToAttributes.Add(GameplayTags.Attributes_Invisible_DexterityAgilityFlexibility, GetDexterityAgilityFlexibilityAttribute);
}

void UHAFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Marksmanship, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Wisdom, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Charisma, COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Flexibility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Purity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Corruptibility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, DarkMajixProficiency, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Intuition, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Vision, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Charm, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, ShieldRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, StaminaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MajixRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxMajix, COND_None, REPNOTIFY_Always);

	//DamageAttributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, IncomingDamage, COND_None, REPNOTIFY_Always);
	// Invisible Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, DexterityAgilityFlexibility, COND_None, REPNOTIFY_Always);

	// Resistance Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Fireproof, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Shockproof, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, ChaosIncorruptible, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Immunity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Unstoppable, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Invulnerability, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, HeartOfDarkness, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, ThermalRadiation, COND_None, REPNOTIFY_Always);
	

	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Majix, COND_None, REPNOTIFY_Always);
}

void UHAFAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const
{
	// --- Extract the context safely ---
	Properties.EffectContextHandle = Data.EffectSpec.GetEffectContext();
	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	// --- Validate Source ASC and its info ---
	if (IsValid(Properties.SourceASC))
	{
		if (Properties.SourceASC->AbilityActorInfo.IsValid())
		{
			Properties.SourceAvatarActor = Properties.SourceASC->GetAvatarActor();
			Properties.SourceController = Properties.SourceASC->AbilityActorInfo->PlayerController.Get();
		}
	}

	// --- Secondary fallback if controller missing but avatar still exists ---
	if (!IsValid(Properties.SourceController) && IsValid(Properties.SourceAvatarActor))
	{
		if (APawn* Pawn = Cast<APawn>(Properties.SourceAvatarActor))
		{
			Properties.SourceController = Pawn->GetController();
		}
	}

	// --- Target info guarded the same way ---
	if (Data.Target.AbilityActorInfo.IsValid())
	{
		Properties.TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();
		Properties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	}

	// --- Logging (safe) ---
	UE_LOG(LogTemp, Log, TEXT("Source ASC: %s | Avatar: %s | Controller: %s"),
		*GetNameSafe(Properties.SourceASC),
		*GetNameSafe(Properties.SourceAvatarActor),
		*GetNameSafe(Properties.SourceController));

	UE_LOG(LogTemp, Log, TEXT("Target ASC: %s | Avatar: %s | Controller: %s"),
		*GetNameSafe(Properties.TargetASC),
		*GetNameSafe(Properties.TargetAvatarActor),
		*GetNameSafe(Properties.TargetController));

	// --- Optional: sanity warning ---
	if (!IsValid(Properties.SourceController))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Missing or invalid SourceController for %s"),
			TEXT(__FUNCTION__), *GetNameSafe(Properties.SourceAvatarActor));
	}

	LogEffectSourceTarget(Properties, TEXT(__FUNCTION__), Data.EffectSpec);
}

void UHAFAttributeSet::LogEffectSourceTarget(const FEffectProperties& Props, const FString& FunctionName, const FGameplayEffectSpec& Spec)
{
	const FString SourceName = GetNameSafe(Props.SourceAvatarActor);
	const FString TargetName = GetNameSafe(Props.TargetAvatarActor);

	// ✅ Extract the name of the GameplayEffect
	const FString EffectName = Spec.Def ? Spec.Def->GetName() : TEXT("UnknownEffect");

	if (Props.SourceASC == Props.TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ⚠️ %s self-applied (%s)"),
			*FunctionName, *SourceName, *EffectName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] ✅ %s ➜ %s (%s)"),
			*FunctionName, *SourceName, *TargetName, *EffectName);
	}

	if (!Props.SourceController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Missing SourceController for %s"), *FunctionName, *SourceName);
	}
	if (!Props.TargetController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Missing TargetController for %s"), *FunctionName, *TargetName);
	}
}

void UHAFAttributeSet::AdjustAttributeForMaxChange(
    FGameplayAttributeData& AffectedAttribute,
    const FGameplayAttributeData& MaxAttribute,
    float NewMaxValue,
    const FGameplayAttribute& AffectedAttributeProperty)
{
    const float CurrentMax = MaxAttribute.GetCurrentValue();
    // Only adjust if we had a valid previous max
    if (!FMath::IsNearlyZero(CurrentMax))
    {
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        const float NewDelta = (CurrentValue * NewMaxValue / CurrentMax) - CurrentValue;

        // Use the GAS API to change the attribute so listeners fire properly
        UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
        if (ASC)
        {
            ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
        }
        else
        {
            // Fallback (rare) if no ASC
            AffectedAttribute.SetBaseValue(CurrentValue + NewDelta);
            AffectedAttribute.SetCurrentValue(CurrentValue + NewDelta);
        }
    }
}


void UHAFAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Clamp vitals to their max
    if (Attribute == GetHealthAttribute())  NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    if (Attribute == GetShieldAttribute())  NewValue = FMath::Clamp(NewValue, 0.f, GetMaxShield());
    if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
    if (Attribute == GetMajixAttribute())   NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMajix());

    // Proportional adjust when Max* changes
    if (Attribute == GetMaxHealthAttribute())
    {
        AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    }
    else if (Attribute == GetMaxShieldAttribute())
    {
        AdjustAttributeForMaxChange(Shield, MaxShield, NewValue, GetShieldAttribute());
    }
    else if (Attribute == GetMaxStaminaAttribute())
    {
        AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
    }
    else if (Attribute == GetMaxMajixAttribute())
    {
        AdjustAttributeForMaxChange(Majix, MaxMajix, NewValue, GetMajixAttribute());
    }
}


void UHAFAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{	
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	// --- Clamp vitals ---
	if (Attribute == GetHealthAttribute())
	{
        // UE_LOG(LogTemp, Warning, TEXT("PostGameplayEffectExecute: Health changed to %f on %s"), GetHealth(), *Props.TargetAvatarActor->GetName());
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	else if (Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	else if (Attribute == GetMajixAttribute())
	{
		SetMajix(FMath::Clamp(GetMajix(), 0.f, GetMaxMajix()));
	}
	else if (Attribute == GetIncomingDamageAttribute())
	{
		const float Damage = GetIncomingDamage();
		// UE_LOG(LogTemp, Warning, TEXT("PostGameplayEffectExecute: IncomingDamage = %f on %s"), Damage, *Props.TargetAvatarActor->GetName());

		SetIncomingDamage(0.f);

		if (Damage > 0.f)
		{
			ApplyDamage(Damage, Props);
		}
	}
}

void UHAFAttributeSet::ApplyDamage(float Damage, const FEffectProperties& Props)
{
	UE_LOG(LogTemp, Warning, TEXT("ApplyDamage called with Damage = %f on %s"), Damage, *Props.TargetAvatarActor->GetName());

	float RemainingDamage = Damage;

	// --- Absorb by shield first ---
	if (GetShield() > 0.f)
	{
		float ShieldBefore = GetShield();
		const float NewShield = FMath::Max(0.f, GetShield() - RemainingDamage);
		float ShieldAbsorbed = ShieldBefore - NewShield;
		RemainingDamage -= (GetShield() - NewShield);
		UE_LOG(LogTemp, Warning, TEXT("Shield absorbed %f damage, new Shield = %f"), ShieldAbsorbed, NewShield);
		SetShield(NewShield);
	}

	// --- Apply leftover to health ---
	if (RemainingDamage > 0.f)
	{
		float HealthBefore = GetHealth();
        float NewHealth = FMath::Clamp(HealthBefore - RemainingDamage, 0.f, GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("Health reduced by %f from %f to %f"), RemainingDamage, HealthBefore, NewHealth);
		SetHealth(NewHealth);
	}

	// --- Handle death or hit reaction ---
	if (GetHealth() <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s died due to damage application"), *Props.TargetAvatarActor->GetName());
		if (ICombatInterface* Combat = Cast<ICombatInterface>(Props.TargetAvatarActor))
		{
			Combat->Die();
		}
	}
	else
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(FHAFGameplayTags::Get().Effects_HitReact);
		Props.TargetASC->TryActivateAbilitiesByTag(Tags);
		UE_LOG(LogTemp, Warning, TEXT("%s took damage and triggered hit react"), *Props.TargetAvatarActor->GetName());
	}

	// --- Floating text or damage numbers are optional ---
	const bool bBlocked  = UHAFAbilitySystemBlueprintLibrary::IsBlockedHit(Props.EffectContextHandle);
	const bool bCritical = UHAFAbilitySystemBlueprintLibrary::IsCriticalHit(Props.EffectContextHandle);
	ShowFloatingText(Props, Damage, bBlocked, bCritical);
}

void UHAFAttributeSet::ShowFloatingText(const FEffectProperties& Properties, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Properties.SourceCharacter != Properties.TargetCharacter)
	{
		if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(UGameplayStatics::GetPlayerController(Properties.SourceCharacter, 0)))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(UGameplayStatics::GetPlayerController(Properties.TargetCharacter, 0)))
		{
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);
		}
		
	}
}

void UHAFAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Health, OldValue);
}

void UHAFAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Shield, OldValue);
}

void UHAFAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Stamina, OldValue);
}

void UHAFAttributeSet::OnRep_Majix(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Majix, OldValue);
}

void UHAFAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Strength, OldValue);
}

void UHAFAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Intelligence, OldValue);
}

void UHAFAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Resilience, OldValue);
}

void UHAFAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Vigor, OldValue);
}

void UHAFAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Dexterity, OldValue);
}

void UHAFAttributeSet::OnRep_Marksmanship(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Marksmanship, OldValue);
}

void UHAFAttributeSet::OnRep_Wisdom(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Wisdom, OldValue);
}

void UHAFAttributeSet::OnRep_Charisma(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Charisma, OldValue);
}

void UHAFAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Armor, OldValue);
}

void UHAFAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ArmorPenetration, OldValue);
}

void UHAFAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, BlockChance, OldValue);
}

void UHAFAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitChance, OldValue);
}

void UHAFAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitDamage, OldValue);
}

void UHAFAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitResistance, OldValue);
}

void UHAFAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Agility, OldValue);
}

void UHAFAttributeSet::OnRep_Flexibility(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Flexibility, OldValue);
}

void UHAFAttributeSet::OnRep_Purity(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Purity, OldValue);
}

void UHAFAttributeSet::OnRep_Corruptibility(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Corruptibility, OldValue);
}

void UHAFAttributeSet::OnRep_DarkMajixProficiency(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, DarkMajixProficiency, OldValue);
}

void UHAFAttributeSet::OnRep_Intuition(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Intuition, OldValue);
}

void UHAFAttributeSet::OnRep_Vision(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Vision, OldValue);
}

void UHAFAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Speed, OldValue);
}

void UHAFAttributeSet::OnRep_Charm(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Charm, OldValue);
}

void UHAFAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, HealthRegeneration, OldValue);
}

void UHAFAttributeSet::OnRep_ShieldRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ShieldRegeneration, OldValue);
}

void UHAFAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, StaminaRegeneration, OldValue);
}

void UHAFAttributeSet::OnRep_MajixRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MajixRegeneration, OldValue);
}

void UHAFAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxHealth, OldValue);
}

void UHAFAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxShield, OldValue);
}

void UHAFAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxStamina, OldValue);
}

void UHAFAttributeSet::OnRep_MaxMajix(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxMajix, OldValue);
}

void UHAFAttributeSet::OnRep_Fireproof(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Fireproof, OldValue);
}

void UHAFAttributeSet::OnRep_ThermalRadiation(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ThermalRadiation, OldValue);
}

void UHAFAttributeSet::OnRep_Shockproof(const FGameplayAttributeData& OldShockproof) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Shockproof, OldShockproof);
}

void UHAFAttributeSet::OnRep_Invulnerability(const FGameplayAttributeData& OldInvulnerability) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Invulnerability, OldInvulnerability);
}

void UHAFAttributeSet::OnRep_ChaosIncorruptible(const FGameplayAttributeData& OldChaosIncorruptible) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ChaosIncorruptible, OldChaosIncorruptible);
}

void UHAFAttributeSet::OnRep_HeartOfDarkness(const FGameplayAttributeData& OldHeartOfDarkness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, HeartOfDarkness, OldHeartOfDarkness);
}

void UHAFAttributeSet::OnRep_Immunity(const FGameplayAttributeData& OldImmunity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Immunity, OldImmunity);
}

void UHAFAttributeSet::OnRep_Unstoppable(const FGameplayAttributeData& OldUnstoppable) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Unstoppable, OldUnstoppable);
}

void UHAFAttributeSet::OnRep_DexterityAgilityFlexibility(const FGameplayAttributeData& OldDexterityAgilityFlexibility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, DexterityAgilityFlexibility, OldDexterityAgilityFlexibility);
}

void UHAFAttributeSet::OnRep_IncomingDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, IncomingDamage, OldValue);
}


