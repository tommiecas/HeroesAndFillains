// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/CapsuleInterface.h"
#include "BaseCharacter.generated.h"

class UGameplayAbility;
struct FGameplayAttribute;
class UGameplayEffect;
class UHAFAttributeSet;
class UHAFAbilitySystemComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class AHAFGameMode;
class UMotionWarpingComponent;

UENUM(BlueprintType, Blueprintable)
enum EDeathPose
{
	EDP_NotDead UMETA(DisplayName = "Not Dead"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	
	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};
UCLASS()
class HEROESANDFILLAINS_API ABaseCharacter : public ACharacter, public IHitInterface, public IAbilitySystemInterface, public ICombatInterface, public ICapsuleInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void MeleeAttack();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual void ReceiveDamage(AActor* DamagedPawn, float DamageAmount, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable)
    virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	
    virtual bool CanAttack();
    
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
    
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

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* DodgeMontage;

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
	virtual void PlayDodgeMontage();
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopAllMontages(float BlendOutTime = 0.25f);
	
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

	UPROPERTY()
	AActor* CachedDamagedPawn;

	UPROPERTY()
	float CachedDamage;

	UPROPERTY()
	const UDamageType* CachedDamageType;

	UPROPERTY()
	AController* CachedInstigatorController;

	UPROPERTY()
	AActor* CachedCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CoreComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AWeaponBase* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class  AMeleeWeapon* EquippedMeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AFillainPlayerController* KillerPlayerController;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* AttributeComponent;

	float LastHitReactTime = 0.f;
	float HitReactCooldown = 0.4f; // seconds

	// Prevents hit react spam
	FTimerHandle HitReactTimer;
	bool bCanReact = true;

	void ResetHitReact();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Warping")
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY()
	AHAFGameMode* HAFGameMode;

	bool bIsEliminated = false;

	void DisableMeshCollision();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UHAFAbilitySystemComponent> HAFAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<UHAFAttributeSet> HAFAttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UHAFAbilitySystemComponent* GetHAFAbilitySystemComponent() const {return HAFAbilitySystemComponent;};
	virtual UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	virtual UHAFAttributeSet* GetHAFAttributeSet() const { return HAFAttributeSet; };

	void MaybeTriggerCharm(AActor* DamagedActor, AActor* DamageInstigator);

	void LogSecondaries_Client() const;
	void LogSecondaries_Server() const;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	virtual FVector GetCombatSocketLocation() override;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo();
	void StopMontage(UAnimMontage* Montage);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* CurrentAttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UEnemyHealthBarWidgetComponent* HealthBarWidgetComponent;

	bool IsCharacterAlive();
	bool bIsCharacterDead{false};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultInvisibleAttributes;

	void ApplyStartupEffects();


	void InitializeDefaultAttributes() const;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	void AddCharacterAbilities();

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

public:
	static float SafeGetNumeric(const UAbilitySystemComponent* ASC,
							const UHAFAttributeSet* AS,
							const FGameplayAttribute& Attr);
	static float SafeGet(const UAbilitySystemComponent* ASC, const UHAFAttributeSet* AS, const FGameplayAttribute& Attr);
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }
	


};