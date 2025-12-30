// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAFDamageGameplayAbility.h"
#include "ArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API UArcaneShards : public UHAFDamageGameplayAbility
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable)
	void ResetShardCount();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArcaneShards")
	int32 ShardCount = 0;
	
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArcaneShards")
	int32 MaxNumberOfShards = 11;

	FORCEINLINE int32 GetShardCount() { return ShardCount; }
	FORCEINLINE void SetShardCount(int32 NewShardCount) { ShardCount = NewShardCount; }
};
