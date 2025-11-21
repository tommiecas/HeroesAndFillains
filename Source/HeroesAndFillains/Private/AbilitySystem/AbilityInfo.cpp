// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityInfo.h"
#include "HeroesAndFillains/HAFLogChannels.h"

FHAFAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FHAFAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogHAF, Error, TEXT("Can't find the AbilityInfo [%s] for AbilityTag [%s]."), *GetNameSafe(this), *AbilityTag.ToString());
	}

	return FHAFAbilityInfo();
}
