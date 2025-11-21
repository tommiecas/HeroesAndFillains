// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SuckStop.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UAnimNotify_SuckStop : public UAnimNotify
{
	GENERATED_BODY()

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

};
