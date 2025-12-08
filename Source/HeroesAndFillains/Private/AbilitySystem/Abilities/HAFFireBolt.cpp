// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"


FString UHAFFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FHAFGameplayTags::Get().Damage_Burn);
	const float MajixCost = FMath::Abs(GetMajixCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		/* READS AS FOLLOWS
		FIRE BOLT
		LEVEL: ##

		Majix Cost: ##
		Cooldown: ##

		Launches ## bolts of fire, exploding on impact
		and dealing ## fire damage
		with a chance to also BURN. 
		 */
		return FString::Printf(TEXT(
			"<Title>FIRE BOLT</>\n "
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Launches </><MoreThanBefore>%d</><Default> bolt of fire, "
			"exploding on impact and dealing: </><Damage>%d</><Default> fire damage "
			"with a chance to also </> <DurationalDamage> Burn </>\n\n "),
			Level,
			MajixCost,
			Cooldown,
			Level,
			Damage * Level);
	}
	else
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT     LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Launches </><MoreThanBefore>%d</><Default> bolts of fire, "
								"exploding on impact and dealing: </><Damage>%d</><Default> fire damage "
								"with a chance to also </> <DurationalDamage> Burn </>\n\n "),
								Level,
								MajixCost,
								Cooldown,
								Level,
								Damage * Level);
	}
}

FString UHAFFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FHAFGameplayTags::Get().Damage_Burn);
	float MajixCost = FMath::Abs(GetMajixCost(Level));
	float Cooldown = GetCooldown(Level);
	if (Level <= 5)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Launches </><MoreThanBefore>%d</>"
								"<Default> bolts of fire, exploding on impact "
								"and dealing: </><Damage>%d</><Default> fire damage "
								"with a chance to also </> <DurationalDamage> Burn </>\n\n "),
								Level,
								MajixCost,
								Cooldown,
								FMath::Min(Level, NumProjectiles),
								Damage * Level);
	}
	else if (Level >5)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Launches </><MoreThanBefore>5</><Default> bolts of fire, "
								"exploding on impact and dealing: </><Damage>%d</><Default> fire damage "
								"with a chance to also </> <DurationalDamage> Burn </>\n\n "),
								Level,
								MajixCost,
								Cooldown,
								Damage * Level);
	}
	return FString();
}

