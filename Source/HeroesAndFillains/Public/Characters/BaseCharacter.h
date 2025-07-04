// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType, Blueprintable)
enum EDeathPose
{
	EDP_NotDead UMETA(DisplayName = "NotDead"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	
	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};
UCLASS()
class HEROESANDFILLAINS_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void MeleeAttack();
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
    virtual void DirectionalHitReact(const FVector& ImpactPoint);
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual float CalculateDamage(AActor* DamagedPawn, float DamageAmount, AController* InstigatorController);


	UFUNCTION(BlueprintCallable)
    virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* EnemyCombat;
	
    virtual bool CanAttack();
    
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();
    
    virtual void CharacterDies();

	/*****************************
    ***                        ***
    ***   ANIMATION MONTAGES   ***
    ***                        ***
    *****************************/

    UPROPERTY(EditDefaultsOnly, Category = Montages)
    class UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, Category = Montages)
    class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(Replicated, EditAnywhere, Category = Combat)
	class UAnimMontage* MeleeAttackMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> MeleeAttackMontageSections;
	


	/*********************************
	***                            ***
	***   PLAY MONTAGE FUNCTIONS   ***
	***                            ***
	*********************************/

	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual int32 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> SectionNames);
	virtual int32 PlayMeleeAttackMontage();
	virtual void PlayHitReactMontage(const FName& SectionName);
    virtual int32 PlayDeathMontage();

	virtual int32 PlayNothingByReturningDefaultMax(TArray<FName> Sections);

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

    UPROPERTY(EditAnywhere, Category = "Sound")
    class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* HitReactParticles;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	class UNiagaraSystem* HitReactSystem;

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitSpecialEffects(const FVector& ImpactPoint);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose = EDeathPose::EDP_NotDead;                   	

	void DisableCapsule();

	UPROPERTY()
	float CachedDamageAmount = 0.f;

	UPROPERTY()
	FDamageEvent CachedDamageEvent;

	UPROPERTY()
	AController* CachedEventInstigator = nullptr;

	UPROPERTY()
	AActor* CachedDamageCauser = nullptr;

	AActor* CachedDamagedPawn;
	float CachedDamage;
	const UDamageType* CachedDamageType;
	AController* CachedInstigatorController;
	AActor* CachedCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AWeaponBase* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AMeleeWeapon* EquippedMeleeWeapon;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* AttributeComponent;

	float LastHitReactTime = 0.f;
	float HitReactCooldown = 0.4f; // seconds

	// Prevents hit react spam
	FTimerHandle HitReactTimer;
	bool bCanReact = true;

	void ResetHitReact();


protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UHealthBarWidgetComponent* NewHealthBarWidgetComponent;

	bool IsCharacterAlive();

private:

public:
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }
	FORCEINLINE UCombatComponent* GetEnemyCombatComponent() const { return EnemyCombat; }
	


};
