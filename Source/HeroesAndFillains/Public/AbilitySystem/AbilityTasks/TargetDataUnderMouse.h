// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

class AEnemyBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&,
                                            DataHandle);
/**
 * 
 */

UCLASS()
class HEROESANDFILLAINS_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	void OnDestroy(bool bInOwnerFinished);

	FTimerHandle HoverTimerHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FVector CursorHitLocation;

	UPROPERTY(BlueprintReadOnly)
	FVector CachedCursorHitLocation;

	UPROPERTY(BlueprintReadOnly)
	FVector FinalLocation;

private:
	virtual void Activate() override;
	void SendMouseCursorData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	UPROPERTY()
	AEnemyBase* LastHoveredEnemy = nullptr;

public:
	FORCEINLINE FVector GetMouseHitLocation() const { return MouseHitLocation; }
	FORCEINLINE FVector GetCursorHitLocation() const { return CursorHitLocation; }
};
	