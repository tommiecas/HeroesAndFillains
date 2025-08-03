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
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxShield);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData Majix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, Majix);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxMajix;
	ATTRIBUTE_ACCESSORS(UHAFAttributeSet, MaxMajix);

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Properties) const;
};
