// AttributeComponent.cpp — FINAL (UE 5.5.4, pure-GAS, const-correct, compile-safe)

#include "HAFComponents/AttributeComponent.h"

#include "AbilitySystem/HAFAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/BaseCharacter.h"
#include "UI/FillainHUD.h"
#include "PlayerController/FillainPlayerController.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

// ---------- Local helpers ----------------------------------------------------

namespace
{
static FORCEINLINE UAbilitySystemComponent* ResolveASCFromOwner(const UObject* ContextObj)
{
	if (!ContextObj) return nullptr;

	const AActor* OwnerActor = Cast<AActor>(ContextObj);
	if (!OwnerActor) return nullptr;

	// Works for ASC on Pawn or on PlayerState
	if (UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(OwnerActor)))
	{
		return ASC;
	}

	// Fallback if your ABaseCharacter exposes a getter
	if (const ABaseCharacter* BC = Cast<ABaseCharacter>(OwnerActor))
	{
		return BC->GetAbilitySystemComponent();
	}

	return nullptr;
}

static FORCEINLINE float GetNumeric(const UObject* ContextObj, const FGameplayAttribute& Attr)
{
	if (const UAbilitySystemComponent* ASC = ResolveASCFromOwner(ContextObj))
	{
		return ASC->GetNumericAttribute(Attr);
	}
	return 0.f;
}
} // namespace

// ---------- UAttributeComponent ---------------------------------------------

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SoulsGathered = 0;
	GoldAcquired  = 0;

	// No caching of UHAFAttributeSet*; reads go through ASC->GetNumericAttribute(...).
	// All writes must be done via GameplayEffects (pure GAS).
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UAttributeComponent::bIsDying() const
{
	const float Health = GetNumeric(this, UHAFAttributeSet::GetHealthAttribute());
	return Health <= 0.f;
}

float UAttributeComponent::GetStamina() const
{
	return GetNumeric(this, UHAFAttributeSet::GetStaminaAttribute());
}

void UAttributeComponent::UpdateTotalSouls(int32 NumberOfSouls)
{
	SoulsGathered += NumberOfSouls;

	if (UWorld* World = GetWorld())
	{
		if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(World->GetFirstPlayerController()))
		{
			PC->SetHUDSoulsCount(SoulsGathered);
		}
	}
}

void UAttributeComponent::UpdateTotalGold(int32 AmountOfGold)
{
	GoldAcquired = FMath::Max(0, GoldAcquired + AmountOfGold);

	if (UWorld* World = GetWorld())
	{
		if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(World->GetFirstPlayerController()))
		{
			PC->SetHUDGoldCount(GoldAcquired);
		}
	}
}

// ---------------- Deprecated (now no-op) ------------------------------------

void UAttributeComponent::CharactersReceiveMeleeDamage(
	float /*DamageAmount*/,
	const FDamageEvent& /*DamageEvent*/,
	AController* /*EventInstigator*/,
	AActor* /*DamageCauser*/)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Warning, TEXT("[AttributeComponent] CharactersReceiveMeleeDamage is deprecated. ")
		TEXT("Apply a GameplayEffect that sets SetByCaller Data.Damage -> IncomingDamage instead."));
#endif
}

void UAttributeComponent::RegenStamina(float /*DeltaTime*/)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Verbose, TEXT("[AttributeComponent] RegenStamina is deprecated. Use a periodic GE for regen."));
#endif
}

// --------------- AttributeSet resolvers (robust across engine APIs) ----------

const UHAFAttributeSet* UAttributeComponent::ResolveAttrSet() const
{
	const UAbilitySystemComponent* LocalASC = ResolveASCFromOwner(this);
	if (!LocalASC) return nullptr;

	// Scan spawned sets to find UHAFAttributeSet (const-correct)
	const TArray<UAttributeSet*>& Sets = LocalASC->GetSpawnedAttributes();
	for (UAttributeSet* Set : Sets)
	{
		if (const UHAFAttributeSet* HAF = Cast<UHAFAttributeSet>(Set))
		{
			return HAF;
		}
	}
	return nullptr;
}

UHAFAttributeSet* UAttributeComponent::ResolveAttrSet()
{
	UAbilitySystemComponent* LocalASC = ResolveASCFromOwner(this);
	if (!LocalASC) return nullptr;

	// Mutable version
	const TArray<UAttributeSet*>& Sets = LocalASC->GetSpawnedAttributes();
	for (UAttributeSet* Set : Sets)
	{
		if (UHAFAttributeSet* HAF = Cast<UHAFAttributeSet>(Set))
		{
			return HAF;
		}
	}
	return nullptr;
}
