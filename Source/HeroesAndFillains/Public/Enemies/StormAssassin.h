// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "StormAssassin.generated.h"


class UBoxComponent;

UENUM(BlueprintType, Blueprintable)
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
UCLASS()
class HEROESANDFILLAINS_API AStormAssassin : public AEnemyBase
{
	GENERATED_BODY()

public:
	AStormAssassin();
	virtual void Tick(float DeltaTime) override;
	virtual void AttackEnd();

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
