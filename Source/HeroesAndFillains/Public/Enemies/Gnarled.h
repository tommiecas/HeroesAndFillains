// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Interfaces/HitInterface.h"
#include "Gnarled.generated.h"


class UBoxComponent;
/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AGnarled : public AEnemyBase
{
	GENERATED_BODY()

public:
	AGnarled();
	virtual void Tick(float DeltaTime) override;
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnFistOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bCanDamage = true;;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FistDamage = 30.f;

	FTimerHandle FistDamageResetTimer;

	UFUNCTION(BlueprintCallable)
	void EnableLeftFist();
	
	UFUNCTION(BlueprintCallable)
	void DisableLeftFist();

	UFUNCTION(BlueprintCallable)
	void EnableRightFist();
	
	UFUNCTION(BlueprintCallable)
	void DisableRightFist();
	
	UFUNCTION(BlueprintCallable)
	void ResetCanDamage();

	UPROPERTY()
	TArray<AActor*> RightFistDamagedActors;

	UPROPERTY()
	TArray<AActor*> LeftFistDamagedActors;
	
	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual int32 PlayDeathMontage() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gender")
	bool bIsGnarledMale = true;

private:
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* RightFistCollision;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* LeftFistCollision;





	
	
};
