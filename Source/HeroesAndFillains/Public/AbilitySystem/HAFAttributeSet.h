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

//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;

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
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	void ApplyInitialValuesForOwner() const;
	void TakeDamageFromShield(float Damage, const FEffectProperties& Properties);
	void TakeDamageFromShieldThenHealth(float Damage, const FEffectProperties& Properties);
	void TakeDamageFromHealth(float Damage, const FEffectProperties& Properties);
	void DealWithDeathAndWidgets(float Damage, const FEffectProperties& Properties); 

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

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, IncomingDamage);

	UFUNCTION() void OnRep_Strength(const FGameplayAttributeData& OldStrength);
	UFUNCTION() void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);
	UFUNCTION() void OnRep_Resilience(const FGameplayAttributeData& OldResilience);
	UFUNCTION() void OnRep_Vigor(const FGameplayAttributeData& OldVigor);
	UFUNCTION() void OnRep_Dexterity(const FGameplayAttributeData& OldDexterity);
	UFUNCTION() void OnRep_Marksmanship(const FGameplayAttributeData& OldMarksmanship);
	UFUNCTION() void OnRep_Wisdom(const FGameplayAttributeData& OldWisdom);
	UFUNCTION() void OnRep_Charisma(const FGameplayAttributeData& OldCharisma);

	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldArmor);
	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration);
	UFUNCTION() void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance);
	UFUNCTION() void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance);
	UFUNCTION() void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage);
	UFUNCTION() void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance);
	UFUNCTION() void OnRep_Agility(const FGameplayAttributeData& OldAgility);
	UFUNCTION() void OnRep_Flexibility(const FGameplayAttributeData& OldFlexibility);
	UFUNCTION() void OnRep_Purity(const FGameplayAttributeData& OldPurity);
	UFUNCTION() void OnRep_Corruptibility(const FGameplayAttributeData& OldCorruptibility);
	UFUNCTION() void OnRep_DarkMajixProficiency(const FGameplayAttributeData& OldDarkMajixProficiency);
	UFUNCTION() void OnRep_Intuition(const FGameplayAttributeData& OldIntuition);
	UFUNCTION() void OnRep_Vision(const FGameplayAttributeData& OldVision); // <-- fix OldSVision -> OldVision
	UFUNCTION() void OnRep_Charm(const FGameplayAttributeData& OldCharm);

	UFUNCTION() void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration);
	UFUNCTION() void OnRep_ShieldRegeneration(const FGameplayAttributeData& OldShieldRegeneration);
	UFUNCTION() void OnRep_StaminaRegeneration(const FGameplayAttributeData& OldStaminaRegeneration);
	UFUNCTION() void OnRep_MajixRegeneration(const FGameplayAttributeData& OldMajixRegeneration);

	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION() void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	UFUNCTION() void OnRep_MaxMajix(const FGameplayAttributeData& OldMaxMajix);

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION() void OnRep_Shield(const FGameplayAttributeData& OldShield);
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION() void OnRep_Majix(const FGameplayAttributeData& OldMajix);

	UFUNCTION() void OnRep_DexterityAgilityFlexibility(const FGameplayAttributeData& OldDexterityAgilityFlexibility);

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const;

	// Proportionally adjust current attribute when its Max changes
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute,
	float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty);
	void ShowFloatingText(const FEffectProperties& Properties, float Damage) const;

public:

};
