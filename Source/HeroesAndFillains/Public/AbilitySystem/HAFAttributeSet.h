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
	void SetAttributeFromComponent(FGameplayAttributeData& Attribute, float AttributeValue);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Health);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Vital Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Shield);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Vital Attributes")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxShield);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Vital Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Stamina);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Vital Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Majix, Category = "Vital Attributes")
	FGameplayAttributeData Majix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Majix);

	UFUNCTION()
	void OnRep_Majix(const FGameplayAttributeData& OldMajix) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMajix, Category = "Vital Attributes")
	FGameplayAttributeData MaxMajix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxMajix);

	UFUNCTION()
	void OnRep_MaxMajix(const FGameplayAttributeData& OldMaxMajix) const;
	
};
