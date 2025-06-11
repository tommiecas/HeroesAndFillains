// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/MeleeTrace.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Characters/FillainCharacter.h"

AMeleeWeapon* UMeleeTrace::GetEquippedWeapon(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp) return nullptr;

	// Assuming your character has a way to access the equipped weapon
	AFillainCharacter* Fillain = Cast<AFillainCharacter>(MeshComp->GetOwner());
	if (!Fillain) return nullptr;

	// Example accessor, adjust this to your actual structure
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(Fillain))
	{
		return Cast<AMeleeWeapon>(FillainCharacter->Combat->EquippedWeapon); 
	}

	return nullptr;
}

void UMeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AMeleeWeapon* Weapon = GetEquippedWeapon(MeshComp))
	{
		Weapon->BeginAttack();
	}
}

void UMeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (AMeleeWeapon* Weapon = GetEquippedWeapon(MeshComp))
	{
		Weapon->TickAttackTrace();
	}
}

void UMeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AMeleeWeapon* Weapon = GetEquippedWeapon(MeshComp))
	{
		Weapon->EndAttack();
	}
}
