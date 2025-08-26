#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "HAFGameplayTags.h"

UHAFAttributeSet::UHAFAttributeSet()
{
	InitStrength(10.f);
	InitIntelligence(17.f);
	InitResilience(12.f);
	InitVigor(10.f);
	InitDexterity(15.f);
	InitMarksmanship(12.f);
	InitWisdom(9.f);
	InitCharisma(13.f);

	const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();

	
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Dexterity, GetDexterityAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Marksmanship, GetMarksmanshipAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Wisdom, GetWisdomAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Charisma, GetCharismaAttribute);

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
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Charm, GetCharmAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ShieldRegeneration, GetShieldRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_StaminaRegeneration, GetStaminaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MajixRegeneration, GetMajixRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxShield, GetMaxShieldAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxStamina, GetMaxStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMajix, GetMaxMajixAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Invisible_DexterityAgilityFlexibility, GetDexterityAgilityFlexibilityAttribute);
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

void UHAFAttributeSet::SetAttributeFromComponent(FGameplayAttributeData& Attribute, float Value)
{
	Attribute.SetBaseValue(Value);
	Attribute.SetCurrentValue(Value);
}

void UHAFAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	if (Attribute == GetShieldAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxShield());
	if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	if (Attribute == GetMajixAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMajix());
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
}

void UHAFAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Properties;
	SetEffectProperties(Data, Properties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetMajixAttribute())
	{
		SetMajix(FMath::Clamp(GetMajix(), 0.f, GetMaxMajix()));
	}
	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
}

void UHAFAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Strength, OldStrength);
}

void UHAFAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Intelligence, OldIntelligence);
}

void UHAFAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Resilience, OldResilience);
}

void UHAFAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Vigor, OldVigor);
}

void UHAFAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Dexterity, OldDexterity);
}

void UHAFAttributeSet::OnRep_Marksmanship(const FGameplayAttributeData& OldMarksmanship) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Marksmanship, OldMarksmanship);
}

void UHAFAttributeSet::OnRep_Wisdom(const FGameplayAttributeData& OldWisdom) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Wisdom, OldWisdom);
}

void UHAFAttributeSet::OnRep_Charisma(const FGameplayAttributeData& OldCharisma) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Charisma, OldCharisma);
}

void UHAFAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Armor, OldArmor);
	UE_LOG(LogTemp, Warning, TEXT("[CLIENT] OnRep_Armor fired: Old=%.2f New=%.2f"),
		static_cast<double>(OldArmor.GetCurrentValue()),
		static_cast<double>(Armor.GetCurrentValue()));
}

void UHAFAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UHAFAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, BlockChance, OldBlockChance);
}

void UHAFAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UHAFAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UHAFAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UHAFAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Agility, OldAgility);
}

void UHAFAttributeSet::OnRep_Flexibility(const FGameplayAttributeData& OldFlexibility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Flexibility, OldFlexibility);
}

void UHAFAttributeSet::OnRep_Purity(const FGameplayAttributeData& OldPurity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Purity, OldPurity);
}

void UHAFAttributeSet::OnRep_Corruptibility(const FGameplayAttributeData& OldCorruptibility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Corruptibility, OldCorruptibility);
}

void UHAFAttributeSet::OnRep_DarkMajixProficiency(const FGameplayAttributeData& OldDarkMajixProficiency) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, DarkMajixProficiency, OldDarkMajixProficiency);
}

void UHAFAttributeSet::OnRep_Intuition(const FGameplayAttributeData& OldIntuition) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Intuition, OldIntuition);
}

void UHAFAttributeSet::OnRep_Vision(const FGameplayAttributeData& OldVision) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Vision, OldVision);
}

void UHAFAttributeSet::OnRep_Charm(const FGameplayAttributeData& OldCharm) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Charm, OldCharm);
}

void UHAFAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UHAFAttributeSet::OnRep_ShieldRegeneration(const FGameplayAttributeData& OldShieldRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, ShieldRegeneration, OldShieldRegeneration);
}

void UHAFAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& OldStaminaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, StaminaRegeneration, OldStaminaRegeneration);
}

void UHAFAttributeSet::OnRep_MajixRegeneration(const FGameplayAttributeData& OldMajixRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MajixRegeneration, OldMajixRegeneration);
}

void UHAFAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxHealth, OldMaxHealth);
}

void UHAFAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxShield, OldMaxShield);
}

void UHAFAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxStamina, OldMaxStamina);
}

void UHAFAttributeSet::OnRep_MaxMajix(const FGameplayAttributeData& OldMaxMajix) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, MaxMajix, OldMaxMajix);
}

void UHAFAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Health, OldHealth);
}

void UHAFAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Shield, OldShield);
}

void UHAFAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Stamina, OldStamina);
}

void UHAFAttributeSet::OnRep_Majix(const FGameplayAttributeData& OldMajix) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, Majix, OldMajix);
}

void UHAFAttributeSet::OnRep_DexterityAgilityFlexibility(
	const FGameplayAttributeData& OldDexterityAgilityFlexibility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAFAttributeSet, DexterityAgilityFlexibility, OldDexterityAgilityFlexibility);
}