// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HAFEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Characters/FillainCharacter.h"
#include "Kismet/GameplayStatics.h"


AHAFEffectActor::AHAFEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Scene Root"));

}

void AHAFEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}
void AHAFEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC || !GameplayEffectClass) return;

	// Carry THIS actor to the effect
	FGameplayEffectContextHandle Ctx = TargetASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, Ctx);
	if (!SpecHandle.IsValid()) return;

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	const FActiveGameplayEffectHandle Handle = TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);

	// (Optional: track infinite effects for removal later — not required for the widget)
	if (Handle.IsValid() && Spec && Spec->Def && Spec->Def->DurationPolicy == EGameplayEffectDurationType::Infinite
		&& InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(Handle, TargetASC);
	}
}

void AHAFEffectActor::OnOverlap(AActor* TargetActor)
{
	if (AFillainCharacter* FC = Cast<AFillainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		FC->OnMessageWidget.Broadcast(ActorLevel);
	}
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AHAFEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (const TPair<FActiveGameplayEffectHandle, TWeakObjectPtr<UAbilitySystemComponent>>& HandlePair : ActiveEffectHandles)
		{
			UAbilitySystemComponent* ASC = HandlePair.Value.Get();
			if (ASC == TargetASC)
			{
				ASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
	if (AFillainCharacter* FC = Cast<AFillainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Destroy();
	}
}