// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/AnimNotify_SuckStop.h"

#include "Enemies/Hellspawn.h"

void UAnimNotify_SuckStop::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AHellspawn* Hellspawn = Cast<AHellspawn>(MeshComp->GetOwner());
	if (!Hellspawn) return;

	Hellspawn->StopZombieFeast();
}

