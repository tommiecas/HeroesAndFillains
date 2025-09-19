#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "HAFGameplayTags.h"
#include "Characters/BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "Enemies/EnemyBase.h"
#include "HAFComponents/CombatComponent.h"

#define ONREP_ATTR(Class, Prop) \
void Class::OnRep_##Prop(const FGameplayAttributeData& Old##Prop) \
{ \
GAMEPLAYATTRIBUTE_REPNOTIFY(Class, Prop, Old##Prop); \
}

UHAFAttributeSet::UHAFAttributeSet() 
{
    // NO GetOwningActor() here.

    const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();

    // Primary
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength,        GetStrengthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence,    GetIntelligenceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience,      GetResilienceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor,           GetVigorAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Dexterity,       GetDexterityAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Marksmanship,    GetMarksmanshipAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Wisdom,          GetWisdomAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Charisma,        GetCharismaAttribute);

    // Secondary
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor,                 GetArmorAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration,      GetArmorPenetrationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance,           GetBlockChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance,     GetCriticalHitChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage,     GetCriticalHitDamageAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Agility,               GetAgilityAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Flexibility,           GetFlexibilityAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Purity,                GetPurityAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Corruptibility,        GetCorruptibilityAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Intuition,             GetIntuitionAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Vision,                GetVisionAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Charm,                 GetCharmAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration,    GetHealthRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ShieldRegeneration,    GetShieldRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_StaminaRegeneration,   GetStaminaRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MajixRegeneration,     GetMajixRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth,             GetMaxHealthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxShield,             GetMaxShieldAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxStamina,            GetMaxStaminaAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMajix,			  GetMaxMajixAttribute);

	// Vital
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health,GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Shield,GetShieldAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Stamina,GetStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Majix,GetMajixAttribute);
	
    // Invisible
    TagsToAttributes.Add(GameplayTags.Attributes_Invisible_DexterityAgilityFlexibility, GetDexterityAgilityFlexibilityAttribute);
}

void UHAFAttributeSet::ApplyInitialValuesForOwner() const 
{
	// Avoid touching owner/ASC if we’re in a CDO tree.
	if (HasAnyFlags(RF_ClassDefaultObject)) return;
	if (const UObject* OuterMost = GetOuter(); OuterMost && OuterMost->HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	const UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const AActor* OwnerActor = ASC ? ASC->GetOwnerActor() : GetTypedOuter<AActor>();

	if (!OwnerActor) return;

	const bool bIsFillain =
		OwnerActor->ActorHasTag("Fillain") || OwnerActor->ActorHasTag("FillainCharacter");
	
}

void UHAFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Marksmanship, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Wisdom, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Charisma, COND_None, REPNOTIFY_Always);

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
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Charm, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, ShieldRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, StaminaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MajixRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, MaxMajix, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, Majix, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UHAFAttributeSet, DexterityAgilityFlexibility, COND_None, REPNOTIFY_Always);
}

void UHAFAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const
{
	Properties.EffectContextHandle = Data.EffectSpec.GetContext();

	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (Properties.SourceASC == nullptr)
	{
		Properties.SourceASC = Properties.EffectContextHandle.GetInstigatorAbilitySystemComponent();
	}

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

	Properties.TargetASC = GetOwningAbilitySystemComponent();
	if (IsValid(Properties.TargetASC) && Properties.TargetASC->AbilityActorInfo.IsValid() && Properties.TargetASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Properties.TargetAvatarActor = Properties.TargetASC->AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Properties.TargetASC->AbilityActorInfo->PlayerController.Get();
		Properties.TargetCharacter = Cast<ACharacter>(Properties.TargetAvatarActor);
	}
	if (!Properties.TargetASC)
	{
		Properties.TargetAvatarActor = nullptr;
		Properties.TargetController  = nullptr;
		Properties.TargetCharacter   = nullptr;
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

	FEffectProperties P;
	SetEffectProperties(Data, P);

	// Clamp vitals if they were modified by an effect
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		if (P.TargetAvatarActor)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Health changed on %s: %.2f"),
				*P.TargetAvatarActor->GetName(), GetHealth());
		}
	}
	if (Data.EvaluatedData.Attribute == GetMajixAttribute())
	{
		SetMajix(FMath::Clamp(GetMajix(), 0.f, GetMaxMajix()));
		if (P.TargetAvatarActor)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Majix changed on %s: %.2f"),
				*P.TargetAvatarActor->GetName(), GetMajix());
		}
	}
	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
		if (P.TargetAvatarActor)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Shield changed on %s: %.2f"),
				*P.TargetAvatarActor->GetName(), GetShield());
		}
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
		if (P.TargetAvatarActor)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Stamina changed on %s: %.2f"),
				*P.TargetAvatarActor->GetName(), GetStamina());
		}
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float Dmg = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (Dmg > 0.f)
		{
			const float NewHP = FMath::Clamp(GetHealth() - Dmg, 0.f, GetMaxHealth());
			SetHealth(NewHP);

			const bool bFatal = (NewHP <= 0.f);
			if (bFatal)
			{
				if (P.TargetAvatarActor)
				{
					if (ICombatInterface* CI = Cast<ICombatInterface>(P.TargetAvatarActor))
					{
						CI->Die();
					}
				}
			}
			else
			{
				// Prefer the target actor rather than GetOwningActor() for hit-reacts
				if (AEnemyBase* Enemy = (P.TargetAvatarActor ? Cast<AEnemyBase>(P.TargetAvatarActor) : nullptr))
				{
					const FVector ImpactPoint = Enemy->GetActorLocation();
					AActor* InstigatorActor = P.SourceAvatarActor ? P.SourceAvatarActor : P.TargetAvatarActor;
					Enemy->GetHit(ImpactPoint, InstigatorActor);

					if (P.TargetASC)
					{
						FGameplayTagContainer Tags;
						Tags.AddTag(FHAFGameplayTags::Get().Effects_HitReact);
						P.TargetASC->TryActivateAbilitiesByTag(Tags);
					}
				}
			}
		}
	}
}

ONREP_ATTR(UHAFAttributeSet, Strength)
ONREP_ATTR(UHAFAttributeSet, Intelligence)
ONREP_ATTR(UHAFAttributeSet, Resilience)
ONREP_ATTR(UHAFAttributeSet, Vigor)
ONREP_ATTR(UHAFAttributeSet, Dexterity)
ONREP_ATTR(UHAFAttributeSet, Marksmanship)
ONREP_ATTR(UHAFAttributeSet, Wisdom)
ONREP_ATTR(UHAFAttributeSet, Charisma)

ONREP_ATTR(UHAFAttributeSet, Armor)
ONREP_ATTR(UHAFAttributeSet, ArmorPenetration)
ONREP_ATTR(UHAFAttributeSet, BlockChance)
ONREP_ATTR(UHAFAttributeSet, CriticalHitChance)
ONREP_ATTR(UHAFAttributeSet, CriticalHitDamage)
ONREP_ATTR(UHAFAttributeSet, CriticalHitResistance)
ONREP_ATTR(UHAFAttributeSet, Agility)
ONREP_ATTR(UHAFAttributeSet, Flexibility)
ONREP_ATTR(UHAFAttributeSet, Purity)
ONREP_ATTR(UHAFAttributeSet, Corruptibility)
ONREP_ATTR(UHAFAttributeSet, DarkMajixProficiency)
ONREP_ATTR(UHAFAttributeSet, Intuition)
ONREP_ATTR(UHAFAttributeSet, Vision)
ONREP_ATTR(UHAFAttributeSet, Charm)

ONREP_ATTR(UHAFAttributeSet, HealthRegeneration)
ONREP_ATTR(UHAFAttributeSet, ShieldRegeneration)
ONREP_ATTR(UHAFAttributeSet, StaminaRegeneration)
ONREP_ATTR(UHAFAttributeSet, MajixRegeneration)

ONREP_ATTR(UHAFAttributeSet, MaxHealth)
ONREP_ATTR(UHAFAttributeSet, MaxShield)
ONREP_ATTR(UHAFAttributeSet, MaxStamina)
ONREP_ATTR(UHAFAttributeSet, MaxMajix)

ONREP_ATTR(UHAFAttributeSet, Health)
ONREP_ATTR(UHAFAttributeSet, Shield)
ONREP_ATTR(UHAFAttributeSet, Stamina)
ONREP_ATTR(UHAFAttributeSet, Majix)

ONREP_ATTR(UHAFAttributeSet, DexterityAgilityFlexibility)