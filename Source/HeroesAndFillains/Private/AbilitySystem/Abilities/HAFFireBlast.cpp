// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFFireBlast.h"

#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Weapons/Majix/HAFFireball.h"

TArray<AHAFFireball*> UHAFFireBlast::SpawnFireballs()
{
	TArray<AHAFFireball*> Fireballs;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UHAFAbilitySystemBlueprintLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireballs);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AHAFFireball* Fireball = GetWorld()->SpawnActorDeferred<AHAFFireball>(FireballClass, SpawnTransform, GetOwningActorFromActorInfo(), CurrentActorInfo->PlayerController->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireball->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireballs.Add(Fireball);
		Fireball->FinishSpawning(SpawnTransform);
	}
	return Fireballs;
}

FString UHAFFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	const float MajixCost = FMath::Abs(GetMajixCost(Level));
	const float Cooldown = GetCooldown(Level);
		/* READS AS FOLLOWS
		FIRE BLAST
		LEVEL: ##

		Majix Cost: ##
		Cooldown: ##

		Launches ## fireballs,
		in all directions,
		that boomerang back,
		exploding upon return,
		causing ## fire damage,
		with a chance to also BURN. 
		 */
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n "
		"<Default>LEVEL: </><Level>%d</>\n\n"
		"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
		"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
		"<Default>Launches twelve fireballs</>\n"
		"<Default>in all directions,</>\n"
		"<Default>that boomerang back,</>\n"
		"<Default>exploding upon return,</>\n"
		"<Default>causing </><Damage>%d</><Default> fire damage</>\n"
		"<Default>with a chance to also </><DurationalDamage> BURN </>\n\n"),
		Level,
		MajixCost,
		Cooldown,
		ScaledDamage * Level);
}

FString UHAFFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
	float MajixCost = FMath::Abs(GetMajixCost(Level));
	float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			"<Title>NEXT LEVEL: </><Level>%d</>\n\n"
			"<Default>Majix Cost: </><MajixCost>%.1f</>\n"
			"<Default>Cooldown></><Cooldown>%.1f</>\n\n"
			"<Default>Launches twelve fireballs</>\n"
			"<Default>in all directions,</>\n"
			"<Default>that boomerang back,</>\n"
			"<Default>exploding upon return,</>\n"
			"<Default>causing </><Damage>%d</><Default> fire damage</>\n"
			"<Default>with a chance to also </><DurationalDamage> BURN </>\n\n"),
			Level,
			MajixCost,
			Cooldown,
			ScaledDamage * Level);
}
