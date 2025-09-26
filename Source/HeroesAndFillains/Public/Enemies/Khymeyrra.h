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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsEnemyKhymeyrra = true;

	virtual void MulticastHandleDeath_Implementation() override;
	virtual void Dissolve() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineZero(UMaterialInstanceDynamic* DynamicMaterialInstanceZero);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineOne(UMaterialInstanceDynamic* DynamicMaterialInstanceOne);

	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineTwo(UMaterialInstanceDynamic* DynamicMaterialInstanceTwo);

	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineThree(UMaterialInstanceDynamic* DynamicMaterialInstanceThree);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineFour(UMaterialInstanceDynamic* DynamicMaterialInstanceFour);

	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineFive(UMaterialInstanceDynamic* DynamicMaterialInstanceFive);

	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineSix(UMaterialInstanceDynamic* DynamicMaterialInstanceSix);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartKhymeyrrahDissolveTimelineSeven(UMaterialInstanceDynamic* DynamicMaterialInstanceSeven);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceThree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceFive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceSix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> KhymeyrrahDissolveMaterialInstanceSeven;

	
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