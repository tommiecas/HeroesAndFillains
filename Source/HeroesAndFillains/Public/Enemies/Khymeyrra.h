// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Khymeyrra.generated.h"

class UBoxComponent;

UCLASS()
class HEROESANDFILLAINS_API AKhymeyrra : public AEnemyBase
{
	GENERATED_BODY()

public:
	AKhymeyrra();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual int32 PlayDeathMontage() override;
	
	virtual void AttackEnd() override;

	UFUNCTION()
	void OnLeftAxeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightAxeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bCanDamage = true;;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AxeDamage = 25.f;

	FTimerHandle AxeDamageResetTimer;
	
	UFUNCTION(BlueprintCallable)
	void EnableLeftAxe();
	
	UFUNCTION(BlueprintCallable)
	void DisableLeftAxe();

	UFUNCTION(BlueprintCallable)
	void EnableRightAxe();
	
	UFUNCTION(BlueprintCallable)
	void DisableRightAxe();
	
	UPROPERTY()
	TArray<AActor*> RightAxeDamagedActors;

	UPROPERTY()
	TArray<AActor*> LeftAxeDamagedActors;

	
private:
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent*  LeftAxeCollision;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent*  RightAxeCollision;

public:


};
