// AttributeComponent.h — FINAL (UE 5.5.4, pure-GAS friendly)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class UAbilitySystemComponent;
class UHAFAttributeSet;
class AController;
class AActor;
struct FDamageEvent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEROESANDFILLAINS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	// ---- GAS-friendly API (no direct writes) ----
	virtual void BeginPlay() override;

	/** Safe check routed through ASC numeric read; no cached AttributeSet used */
	bool bIsDying() const;

	/** Example reader implemented via ASC->GetNumericAttribute in the .cpp */
	float GetStamina() const;

	/** Non-GAS bookkeeping you’re showing on the HUD */
	void UpdateTotalSouls(int32 NumberOfSouls);
	void UpdateTotalGold(int32 AmountOfGold);

	/** Kept for compatibility with existing call sites; now a no-op in .cpp */
	void CharactersReceiveMeleeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
									 AController* EventInstigator, AActor* DamageCauser);

	/** Deprecated: stamina regen should be handled by a periodic GameplayEffect */
	void RegenStamina(float DeltaTime);

	/** Resolve the AttributeSet without caching; const/non-const overloads */
	const UHAFAttributeSet* ResolveAttrSet() const;
	UHAFAttributeSet*       ResolveAttrSet();

public:
	// ---- Non-GAS attributes/state you still own ----
	
	/** Currency/counter values you display on HUD (not GAS-backed) */
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 GoldAcquired = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 SoulsGathered = 0;

	/** Gameplay tuning knobs that are not GAS attributes */
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

	FORCEINLINE int32 GetSoulsGathered() const { return SoulsGathered; }
	FORCEINLINE int32 GetGoldAcquired() const { return GoldAcquired; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStaminaRegenRate() const { return StaminaRegenRate; }
};
