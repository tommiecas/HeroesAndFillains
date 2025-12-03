// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HAFAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define ATTRIBUTE_CAPTUREDEF(PropertyName) \
public: \
static const FGameplayEffectAttributeCaptureDefinition& Get##PropertyName##Capture() \
{ \
static const FGameplayEffectAttributeCaptureDefinition CaptureDef( \
UHAFAttributeSet::Get##PropertyName##Attribute(), \
EGameplayEffectAttributeCaptureSource::Source, \
true \
); \
return CaptureDef; \
}

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

// typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UHAFAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UHAFAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	void ApplyDamage(float Damage, const FEffectProperties& Properties);

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	
	/*********************************
	****    PRIMARY ATTRIBUTES    ****
	**********************************/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Intelligence);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Vigor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dexterity, Category = "Primary Attributes")
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Dexterity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Marksmanship, Category = "Primary Attributes")
	FGameplayAttributeData Marksmanship;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Marksmanship);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Wisdom, Category = "Primary Attributes")
	FGameplayAttributeData Wisdom;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Wisdom);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Charisma, Category = "Primary Attributes")
	FGameplayAttributeData Charisma;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Charisma);
	
	/***********************************
    ****    SECONDARY ATTRIBUTES    ****
    ***********************************/
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, BlockChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, CriticalHitChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, CriticalHitResistance)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Agility, Category = "Secondary Attributes")
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Agility);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Flexibility, Category = "Secondary Attributes")
	FGameplayAttributeData Flexibility;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Flexibility);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Purity, Category = "Secondary Attributes")
	FGameplayAttributeData Purity;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Purity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Corruptibility, Category = "Secondary Attributes")
	FGameplayAttributeData Corruptibility;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Corruptibility);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DarkMajixProficiency, Category = "Secondary Attributes")
	FGameplayAttributeData DarkMajixProficiency;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, DarkMajixProficiency);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intuition, Category = "Secondary Attributes")
	FGameplayAttributeData Intuition;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Intuition);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vision, Category = "Secondary Attributes")
	FGameplayAttributeData Vision;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Vision);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "Secondary Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Speed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Charm, Category = "Secondary Attributes")
	FGameplayAttributeData Charm;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Charm);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, HealthRegeneration);;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData ShieldRegeneration;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, ShieldRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData StaminaRegeneration;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, StaminaRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MajixRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData MajixRegeneration;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MajixRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Vital Attributes")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxShield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Vital Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMajix, Category = "Vital Attributes")
	FGameplayAttributeData MaxMajix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxMajix);

	/************************************
	****    RESISTANCE ATTRIBUTES    ****
	************************************/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Fireproof, Category = "Resistance Attributes")
	FGameplayAttributeData Fireproof;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Fireproof);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shockproof, Category = "Resistance Attributes");
	FGameplayAttributeData Shockproof;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Shockproof);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosIncorruptible, Category = "Resistance Attributes");
	FGameplayAttributeData ChaosIncorruptible;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, ChaosIncorruptible);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ThermalRadiation, Category = "Resistance Attributes");
	FGameplayAttributeData ThermalRadiation;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, ThermalRadiation);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HeartOfDarkness, Category = "Resistance Attributes");
	FGameplayAttributeData HeartOfDarkness;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, HeartOfDarkness);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Invulnerability, Category = "Resistance Attributes");
	FGameplayAttributeData Invulnerability;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Invulnerability);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Unstoppable, Category = "Resistance Attributes");
	FGameplayAttributeData Unstoppable;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Unstoppable);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Immunity, Category = "Resistance Attributes");
	FGameplayAttributeData Immunity;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Immunity);

	
	
	/*******************************
	****    VITAL ATTRIBUTES    ****
	*******************************/
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Vital Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Shield);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Vital Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Stamina);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Majix, Category = "Vital Attributes")
	FGameplayAttributeData Majix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Majix);

	/***********************************
	****    INVISIBLE ATTRIBUTES    ****
	***********************************/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DexterityAgilityFlexibility, Category = "Invisible Attributes")
	FGameplayAttributeData DexterityAgilityFlexibility;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, DexterityAgilityFlexibility);

	/******************************
	****    META ATTRIBUTES    ****
	******************************/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IncomingDamage, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, IncomingXP);


	// CaptureDefinitions
	ATTRIBUTE_CAPTUREDEF(Strength)
	ATTRIBUTE_CAPTUREDEF(Dexterity)
	ATTRIBUTE_CAPTUREDEF(Vigor)
	ATTRIBUTE_CAPTUREDEF(Intelligence)
	ATTRIBUTE_CAPTUREDEF(Resilience)
	ATTRIBUTE_CAPTUREDEF(Marksmanship)
	ATTRIBUTE_CAPTUREDEF(Wisdom)
	ATTRIBUTE_CAPTUREDEF(Charisma)
	ATTRIBUTE_CAPTUREDEF(Health)
	ATTRIBUTE_CAPTUREDEF(MaxHealth)
	ATTRIBUTE_CAPTUREDEF(Stamina)
	ATTRIBUTE_CAPTUREDEF(MaxStamina)
	ATTRIBUTE_CAPTUREDEF(Shield)
	ATTRIBUTE_CAPTUREDEF(MaxShield)
	ATTRIBUTE_CAPTUREDEF(Majix)
	ATTRIBUTE_CAPTUREDEF(MaxMajix)

	
	UFUNCTION() void OnRep_Strength(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Intelligence(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Resilience(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Vigor(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Dexterity(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Marksmanship(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Wisdom(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Charisma(const FGameplayAttributeData& OldValue) const;

	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_BlockChance(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Agility(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Flexibility(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Purity(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Corruptibility(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_DarkMajixProficiency(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Intuition(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Vision(const FGameplayAttributeData& OldValue) const; // <-- fix OldSVision -> OldVision
	UFUNCTION() void OnRep_Speed(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Charm(const FGameplayAttributeData& OldValue) const;

	UFUNCTION() void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_ShieldRegeneration(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_StaminaRegeneration(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_MajixRegeneration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION() void OnRep_Fireproof(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Shockproof(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_ChaosIncorruptible(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Invulnerability(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_HeartOfDarkness(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_ThermalRadiation(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Unstoppable(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Immunity(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_MaxShield(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_MaxMajix(const FGameplayAttributeData& OldValue) const;

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Shield(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_Majix(const FGameplayAttributeData& OldValue) const;

	UFUNCTION() void OnRep_IncomingDamage(const FGameplayAttributeData& OldValue) const;
	UFUNCTION() void OnRep_DexterityAgilityFlexibility(const FGameplayAttributeData& OldValue) const;

protected:
	static void LogEffectSourceTarget(const FEffectProperties& Props, const FString& FunctionName, const FGameplayEffectSpec& Spec);

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const;
	// Proportionally adjust current attribute when its Max changes
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute,
	float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty);
	void ShowFloatingText(const FEffectProperties& Properties, float Damage, bool bBlockedHit, bool bCriticalHit) const;
	void SendXPEvent(const FEffectProperties& Properties);
	bool bTopOffHealth = false;
	bool bTopOffShield = false;
	bool bTopOffStamina = false;
	bool bTopOffMajix = false;
public:

};
