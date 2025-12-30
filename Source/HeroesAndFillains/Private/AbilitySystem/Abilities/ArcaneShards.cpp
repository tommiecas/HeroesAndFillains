// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"


void UArcaneShards::ResetShardCount()
{
	if (ShardCount)
	{
		SetShardCount(0);
	}
}

FString UArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	const float MajixCost = FMath::Abs(GetMajixCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		/* READS AS FOLLOWS
		ARCANE SHARDS
		(A CORRUPT CHAOS MAJIX SPELL!)
		LEVEL: ##

		Majix Cost: ##
		Cooldown: ##

		Summons ## jagged shards
		of corrupt Chaos Majix,
		striking those in its wake
		with ## radial arcane damage...
		causing dire repercussions for not only the victims
		...but the spellcaster as well.
		 */
		return FString::Printf(TEXT(
			"<Title>ARCANE SHARDS</>\n "
			"<ChaosMajix>A CHAOS MAJIX SPELL!</>\n"
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Summon </><MoreThanBefore>%d</><Default> jagged shard</>\n"
			"<Default> of vile, corrupt Chaos Majix,</>\n"
			"<Default> striking those in its wake</>\n"
			"<Default> with <Damage>%d</><Default> radial arcane energy</>\n"
			"<Default> causing dire repercussions for not only the victims...</>\n"
			"<Default>...but the spellcaster as well...</>\n"),
			Level,
			MajixCost,
			Cooldown,
			FMath::Min(Level, MaxNumberOfShards),
			ScaledDamage * Level);
	}
	else if (Level > 1 && Level <= MaxNumberOfShards)
	{
		return FString::Printf(TEXT(
			"<Title>ARCANE SHARDS</>\n "
			"<ChaosMajix>A CHAOS MAJIX SPELL!</>\n"
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Summons </><MoreThanBefore>%d</><Default> jagged shards</>\n"
			"<Default> of vile, corrupt Chaos Majix,</>\n"
			"<Default> striking those in their wake</>\n"
			"<Default> with <Damage>%d</><Default> radial arcane energy</>\n"
			"<Default> causing dire repercussions for not only the victims...</>\n"
			"<Default>...but the spellcaster as well...</>\n"),
			Level,
			MajixCost,
			Cooldown,
			FMath::Min(Level, MaxNumberOfShards),
			ScaledDamage * Level);
	}
	else if (Level > MaxNumberOfShards)
	{
		return FString::Printf(TEXT(
			"<Title>ARCANE SHARDS</>\n "
			"<ChaosMajix>A CHAOS MAJIX SPELL!</>\n"
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Summons eleven jagged shards</>\n"
			"<Default> of vile, corrupt Chaos Majix,</>\n"
			"<Default> striking those in their wake</>\n"
			"<Default> with <Damage>%d</><Default> radial arcane energy</>\n"
			"<Default> causing dire repercussions for not only the victims...</>\n"
			"<Default>...but the spellcaster as well...</>\n"),
			Level,
			MajixCost,
			Cooldown,
			ScaledDamage * Level);
	}
	return FString();
}

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	float MajixCost = FMath::Abs(GetMajixCost(Level));
	float Cooldown = GetCooldown(Level);
	if (Level <= MaxNumberOfShards)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Summons </><MoreThanBefore>%d</><Default> jagged shards</>\n"
								"<Default> of vile, corrupt Chaos Majix,</>\n"
								"<Default> striking those in their wake</>\n"
								"<Default> with <Damage>%d</><Default> radial arcane energy</>\n"
								"<Default> causing dire repercussions for not only the victims...</>\n"
								"<Default>...but the spellcaster as well...</>\n"),
								Level,
								MajixCost,
								Cooldown,
								FMath::Min(Level, MaxNumberOfShards),
								ScaledDamage * Level);
	}
	else if (Level > MaxNumberOfShards)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Summons eleven jagged shards</>\n"
								"<Default> of vile, corrupt Chaos Majix,</>\n"
								"<Default> striking those in their wake</>\n"
								"<Default> with <Damage>%d</><Default> radial arcane energy</>\n"
								"<Default> causing dire repercussions for not only the victims...</>\n"
								"<Default>...but the spellcaster as well...</>\n"),
								Level,
								MajixCost,
								Cooldown,
								ScaledDamage * Level);
	}
	return FString();
}


