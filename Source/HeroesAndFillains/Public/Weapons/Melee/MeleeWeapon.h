// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponTypes.h"
#include "MeleeWeapon.generated.h"

class USphereComponent;

UCLASS()
class HEROESANDFILLAINS_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	AMeleeWeapon();
	virtual void EnableCustomDepth(bool bEnable) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Equip(USceneComponent* InParent, FName InSocketName) override;
	virtual void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName) override;
	void BeginAttack();
	void TickAttackTrace();
	void TraceBetweenPoints(FVector& LastLocation, USceneComponent* TracePoint);
	void EndAttack();

	UFUNCTION()
	void SetEquippedMeleeWeaponState();

	UFUNCTION(BlueprintCallable)
	void SetMeleeWeaponInformationText(UWidgetComponent* MeleeWidgetComponent, AMeleeWeapon* MeleeWeapon);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponHistory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponResistances;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponWeaknesses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FString MeleeWeaponDamage;

	UPROPERTY()
	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	virtual void OnEquippedOneHanded() override;
	virtual void OnEquippedTwoHanded() override;
	virtual void OnDropped() override;
	virtual void OnEquippedSecondary() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Trace")
	class USceneComponent* TracePointTip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Trace")
	USceneComponent* TracePointMid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Trace")
	USceneComponent* TracePointHilt;

	UPROPERTY()
	bool bIsTracing = false;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	
	
private:
	void NativeConstruct();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UBoxComponent* WeaponBox;
	
	FVector LastTraceLocationTip;
	FVector LastTraceLocationMid;
	FVector LastTraceLocationHilt;


	

	

public:
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeType; }
	FORCEINLINE UItemInfoWidgetBase* GetItemInfoWidget() const { return ItemInfoWidget; }
	FORCEINLINE USceneComponent* GetTracePointTip() const { return TracePointTip; }
	FORCEINLINE USceneComponent* GetTracePointMid() const { return TracePointMid; }
	FORCEINLINE USceneComponent* GetTracePointHilt() const { return TracePointHilt; }
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE FVector GetLastTraceLocationTip() const { return LastTraceLocationTip; }
	FORCEINLINE FVector GetLastTraceLocationMid() const { return LastTraceLocationMid; }
	FORCEINLINE FVector GetLastTraceLocationHilt() const { return LastTraceLocationHilt; }
	

	

};



