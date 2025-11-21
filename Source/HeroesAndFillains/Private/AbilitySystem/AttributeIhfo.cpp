// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeIhfo.h"

#include "HeroesAndFillains/HAFLogChannels.h"

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
		UE_LOG(LogHAF, Error, TEXT("Can't find the Info for AttributeTag [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FHAFAttributeInfo();
}

