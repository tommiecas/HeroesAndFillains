// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Feratta.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AFeratta : public AEnemyBase
{
	GENERATED_BODY()

public:
	AFeratta();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> FerattaStaff;

	virtual void SpawnEnemyWeapon_Implementation() override;
protected:
	void OnAttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void ResetCanDamage() override;
	virtual void Dissolve() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineOne(UMaterialInstanceDynamic* DynamicMaterialInstanceOne);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineTwo(UMaterialInstanceDynamic* DynamicMaterialInstanceTwo);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineThree(UMaterialInstanceDynamic* DynamicMaterialInstanceThree);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineFour(UMaterialInstanceDynamic* DynamicMaterialInstanceFour);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineFive(UMaterialInstanceDynamic* DynamicMaterialInstanceFive);

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimelineSix(UMaterialInstanceDynamic* DynamicMaterialInstanceSix);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceTwo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceThree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceFour;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceFive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstanceSix;

	virtual int32 PlayDeathMontage() override;

};
