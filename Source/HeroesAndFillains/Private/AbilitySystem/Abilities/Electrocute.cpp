// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	const float MajixCost = FMath::Abs(GetMajixCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		/* READS AS FOLLOWS
		ELECTROCUTE
		LEVEL: ##

		Majix Cost: ##
		Cooldown: ##

		Launches ## bolts of lightning,
		which then chain through
		## nearby enemies,
		dealing ## lightning damage
		with a chance to also SHOCK. 
		 */
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n "
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Emits a beam of lightning,</>\n"
			"<Default> which then chains through</>\n"
			"<MoreThanBefore>%d</><Default> nearby enemy,</>\n"
			"<Default> dealing </><Damage>%d</><Default> damage,</>\n"
			"<Default> with a chance to also </><DurationalDamage> STUN </>\n\n"),
			Level,
			MajixCost,
			Cooldown,
			FMath::Min(Level, NumShockTargetsMax),
			ScaledDamage * Level);
	}
	else if (Level > 1 && Level <= NumShockTargetsMax)
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n "
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Emits a beam of lightning,</>\n"
			"<Default> which then chains through</>\n"
			"<MoreThanBefore>%d</><Default> nearby enemies,</>\n"
			"<Default> dealing </><Damage>%d</><Default> damage,</>\n"
			"<Default> with a chance to also </><DurationalDamage> STUN </>\n\n"),
			Level,
			MajixCost,
			Cooldown,
			FMath::Min(Level, NumShockTargetsMax),
			ScaledDamage * Level);
	}
	else if (Level > NumShockTargetsMax)
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n "
			"<Default>LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Emits a beam of lightning,</>\n"
			"<Default>which then chains through</>\n"
			"<Default>ten nearby enemies,</>\n"
			"<Default> dealing </><Damage>%d</><Default> damage,</>\n"
			"<Default> with a chance to also </><DurationalDamage> STUN </>\n\n"),
			Level,
			MajixCost,
			Cooldown,
			ScaledDamage * Level);
	}
	return FString();
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	float MajixCost = FMath::Abs(GetMajixCost(Level));
	float Cooldown = GetCooldown(Level);
	if (Level <= NumShockTargetsMax)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
							  "<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Emits a beam of lightning,</>\n"
								"<Default> which then chains through</>\n"
								"<MoreThanBefore>%d</><Default> nearby enemies,</>\n"
								"<Default> dealing </><Damage>%d</><Default> damage,</>\n"
								"<Default> with a chance to also </><DurationalDamage> STUN </>\n\n"),
								Level,
								MajixCost,
								Cooldown,
								FMath::Min(Level, NumShockTargetsMax),
								ScaledDamage * Level);
	}
	else if (Level > NumShockTargetsMax)
	{
		return FString::Printf(TEXT("<Title>NEXT LEVEL: </><Level>%d</>\n\n"
								"<Default>Majix Cost:</><MajixCost>%.1f</>\n"
								"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
								"<Default>Emits a beam of lightning,</>\n"
								"<Default> which then chains through</>\n"
								"<Default> ten nearby enemies,</>\n"
								"<Default> dealing </><Damage>%d</><Default> damage,</>\n"
								"<Default> with a chance to also </><DurationalDamage> STUN </>\n\n"),
								Level,
								MajixCost,
								Cooldown,
								ScaledDamage * Level);
	}
	return FString();
}


