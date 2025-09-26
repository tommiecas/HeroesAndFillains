// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "StormAssassin.generated.h"


class UBoxComponent;

UENUM(BlueprintType, Blueprintable, meta = (ScriptName = "E_StormAssassin"))
enum class EStormAssassin : uint8
{
	ESA_Sandstorm UMETA(DisplayName = "Sandstorm"),
	ESA_Soulstorm UMETA(DisplayName = "Soulstorm"),
	ESA_Skystorm UMETA(DisplayName = "Skystorm"),
	ESA_Shadowstorm UMETA(DisplayName = "Shadowstorm"),

	ESA_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS(meta = (ScriptName = "U_StormAssassin"))
class HEROESANDFILLAINS_API AStormAssassin : public AEnemyBase
{
	GENERATED_BODY()

public:
	AStormAssassin();
	virtual void Tick(float DeltaTime) override;
	virtual void AttackEnd() override;
	virtual void Die() override;
	virtual void MulticastHandleDeath_Implementation() override;
	virtual void Dissolve() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineZero(UMaterialInstanceDynamic* DynamicMaterialInstanceZero);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineOne(UMaterialInstanceDynamic* DynamicMaterialInstanceOne);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineTwo(UMaterialInstanceDynamic* DynamicMaterialInstanceTwo);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineThree(UMaterialInstanceDynamic* DynamicMaterialInstanceThree);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineFour(UMaterialInstanceDynamic* DynamicMaterialInstanceFour);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineFive(UMaterialInstanceDynamic* DynamicMaterialInstanceFive);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineSix(UMaterialInstanceDynamic* DynamicMaterialInstanceSix);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineSeven(UMaterialInstanceDynamic* DynamicMaterialInstanceSeven);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineEight(UMaterialInstanceDynamic* DynamicMaterialInstanceEight);

	UFUNCTION(BlueprintImplementableEvent)
	void StartStormDissolveTimelineNine(UMaterialInstanceDynamic* DynamicMaterialInstanceNine);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceThree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceFive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceSix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceSeven;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceEight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceNine;


protected:
	virtual void BeginPlay() override;
	
	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual int32 PlayDeathMontage() override;

	UFUNCTION(BlueprintCallable)
	void OnLeftFootOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnRightFootOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bCanDamage = true;;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FootDamage = 15.f;

	FTimerHandle FootDamageResetTimer;

	UFUNCTION(BlueprintCallable)
	void EnableLeftFoot();
	
	UFUNCTION(BlueprintCallable)
	void DisableLeftFoot();

	UFUNCTION(BlueprintCallable)
	void EnableRightFoot();
	
	UFUNCTION(BlueprintCallable)
	void DisableRightFoot();

	UPROPERTY()
	TArray<AActor*> RightFootDamagedActors;

	UPROPERTY()
	TArray<AActor*> LeftFootDamagedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EStormAssassin StormAssassin = EStormAssassin::ESA_MAX;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* RightFootCollision;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* LeftFootCollision;
	
	
};