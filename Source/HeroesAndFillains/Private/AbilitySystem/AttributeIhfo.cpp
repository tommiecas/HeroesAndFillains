// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeIhfo.h"

FHAFAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FHAFAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find the Info for AttributeTag [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FHAFAttributeInfo();
}

