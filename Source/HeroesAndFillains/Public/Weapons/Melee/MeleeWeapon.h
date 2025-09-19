// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponBase.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
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
	virtual void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator) override;
	virtual void BeginAttack();
	virtual void ImplementLineTraceGetHit(FHitResult Hit);
	virtual void TickAttackTrace();
	virtual void TraceBetweenPoints(FVector& LastLocation, USceneComponent* TracePoint);
	virtual void EndAttack();

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION()
	void SetEquippedMeleeWeaponState();

	UFUNCTION(BlueprintCallable)
	void SetMeleeWeaponInformationText(UWidgetComponent* MeleeItemInfoComp, AMeleeWeapon* MeleeWeapon);
	
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

	bool ActorIsSameType(AActor* OtherActor);

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;



	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
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
	/******************************
	****    VISION-IMPACTED    ****
	****    ENEMY "CAPSULE"    ****
	****      ADJUSTMENT       ****
	******************************/
	
	UPROPERTY(EditDefaultsOnly, Category="Melee|Assist")
	float BaseTraceRadiusCM = 4.f;      // your “real” blade thickness

	UPROPERTY(Transient)
	float CachedVisionPadCM = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float MeleeDamage;


	
	FVector LastTraceLocationTip;
	FVector LastTraceLocationMid;
	FVector LastTraceLocationHilt;


	

	

public:
	FORCEINLINE EMeleeType GetMeleeWeaponType() const { return MeleeType; }
	FORCEINLINE UUserWidget* GetItemInfoWidget() const { return ItemInfoWidget; }
	FORCEINLINE USceneComponent* GetTracePointTip() const { return TracePointTip; }
	FORCEINLINE USceneComponent* GetTracePointMid() const { return TracePointMid; }
	FORCEINLINE USceneComponent* GetTracePointHilt() const { return TracePointHilt; }
	FORCEINLINE FVector GetLastTraceLocationTip() const { return LastTraceLocationTip; }
	FORCEINLINE FVector GetLastTraceLocationMid() const { return LastTraceLocationMid; }
	FORCEINLINE FVector GetLastTraceLocationHilt() const { return LastTraceLocationHilt; }
	

	

};