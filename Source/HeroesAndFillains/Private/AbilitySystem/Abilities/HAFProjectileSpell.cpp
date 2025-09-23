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

        if (const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
        {
            FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
            Context.AddSourceObject(Projectile);

            FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);

            if (SpecHandle.IsValid())
            {
                const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

                // ✅ Correctly assign the SetByCaller.Damage tag
                UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
                    SpecHandle,
                    FGameplayTag::RequestGameplayTag(FName("SetByCaller.Damage")),
                    ScaledDamage
                );

                Projectile->DamageEffectSpecHandle = SpecHandle;

                UE_LOG(LogTemp, Log, TEXT("Projectile spawned with valid spec for %s | Damage: %.2f"),
                    *DamageEffectClass->GetName(),
                    ScaledDamage);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Projectile spawn failed: invalid DamageEffectSpecHandle! DamageEffectClass = %s"),
                    DamageEffectClass ? *DamageEffectClass->GetName() : TEXT("nullptr"));
            }
        }

        Projectile->FinishSpawning(SpawnTransform);
    }
}
