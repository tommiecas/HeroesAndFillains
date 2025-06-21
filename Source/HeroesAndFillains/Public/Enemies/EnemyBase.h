// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "EnemyBase.generated.h"

UCLASS()
class HEROESANDFILLAINS_API AEnemyBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void EnemyDies();

	/*****************************
    ***                        ***
    ***   ANIMATION MONTAGES   ***
    ***                        ***
    *****************************/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	class UNiagaraSystem* HitReactSystem;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;
	
protected:
	virtual void BeginPlay() override;

	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void PlayDeathMontage();

	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundBase* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UHealthBarWidgetComponent* NewHealthBarWidgetComponent;

	

private:
	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* AttributeComponent;



	
private:

	
	
public:	
	

};
