// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HAFProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Weapons/Majix/HAFMajixProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "HAFGameplayTags.h"


void UHAFProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

FGameplayEffectContextHandle UHAFProjectileSpell::AddSourceObjectToContext(const FGameplayEffectContextHandle& Context,
	UObject* SourceObject)
{
	FGameplayEffectContextHandle NewContext = Context;
	NewContext.AddSourceObject(SourceObject);
	return NewContext;
}

void UHAFProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
    {
        const FVector SocketLocation = CombatInterface->GetSpellCastersSocketLocation();
        FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
        Rotation.Pitch = 0.f;

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);
        SpawnTransform.SetRotation(Rotation.Quaternion());

        AHAFMajixProjectile* Projectile = GetWorld()->SpawnActorDeferred<AHAFMajixProjectile>(
            HAFMajixProjectileClass,
            SpawnTransform,
            GetOwningActorFromActorInfo(),
            Cast<APawn>(GetOwningActorFromActorInfo()),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
        FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
    	EffectContextHandle.SetAbility(this);
    	EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
    	Actors.Add(Projectile);
    	EffectContextHandle.AddActors(Actors);
    	FHitResult HitResult;
    	HitResult.Location = ProjectileTargetLocation;
    	EffectContextHandle.AddHitResult(HitResult);

        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);;
        const FHAFGameplayTags GameplayTags = FHAFGameplayTags::Get();

    	for (auto& Pair : DamageTypes)
    	{
    		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
    		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    	}

        Projectile->DamageEffectSpecHandle = SpecHandle;
    	Projectile->FinishSpawning(SpawnTransform);
    }
}
