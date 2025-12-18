// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "HAFGameplayTags.h"
#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/FillainCharacter.h"
#include "Weapons/Majix/HAFMajixProjectile.h"


FString UHAFFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
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
			ScaledDamage * Level);
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
								ScaledDamage * Level);
	}
}

FString UHAFFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageAmount.GetValueAtLevel(Level);
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
								ScaledDamage * Level);
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
								ScaledDamage * Level);
	}
	return FString();
}

void UHAFFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	if (AFillainCharacter* Fill = Cast<AFillainCharacter>(GetOwningActorFromActorInfo()))
	{
		FVector SocketLocation = Fill->GetMesh()->GetSocketLocation(TEXT("SpellSocket"));
		ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag) = SocketLocation;;
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bOverridePitch) Rotation.Pitch = PitchOverride;

		const FVector Forward = Rotation.Vector();
		const int32 NumFirebolts = FMath::Min(MaxProjectiles, GetAbilityLevel());
		TArray<FRotator> Rotations = UHAFAbilitySystemBlueprintLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumFirebolts);

		for (const FRotator& Rot : Rotations)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rot.Quaternion());

			AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(HAFMajixProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

			if (IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>())
			{
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				USceneComponent* TempComp = NewObject<USceneComponent>(Projectile);
				TempComp->RegisterComponent();
				TempComp->AttachToComponent(Projectile->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

				Projectile->HomingTargetSceneComponent = TempComp;
				Projectile->ProjectileMovement->HomingTargetComponent = TempComp;
			}

			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
			Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
			
			Projectile->FinishSpawning(SpawnTransform);
		}

	}
}

