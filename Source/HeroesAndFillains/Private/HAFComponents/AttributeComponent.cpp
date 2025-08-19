#include "HAFComponents/AttributeComponent.h"

#include "Characters/FillainCharacter.h"
#include "HUD/FillainHUD.h"
#include "PlayerController/FillainPlayerController.h"
#include "GameFramework/DamageType.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AttributeSet = nullptr; 
	SoulsGathered = 0;
	GoldAcquired = 0; // Ensure we start from a defined value
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

float UAttributeComponent::GetStamina() const
{
	if (const UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		return ASC->GetNumericAttribute(UHAFAttributeSet::GetStaminaAttribute());
	}
	return 0.f;
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	AttributeSet->SetStamina(FMath::Clamp(AttributeSet->GetStamina() + StaminaRegenRate * DeltaTime, 0.f, AttributeSet->GetMaxStamina()));
}

void UAttributeComponent::UpdateTotalSouls(int32 NumberOfSouls)
{
	SoulsGathered += NumberOfSouls;
	UE_LOG(LogTemp, Warning, TEXT("SoulsGathered is now: %d"), SoulsGathered);

	// Update HUD
	if (UWorld* World = GetWorld())
	{
		if (AFillainPlayerController* FillainPlayerController = Cast<AFillainPlayerController>(World->GetFirstPlayerController()))
		{
			FillainPlayerController->SetHUDSoulsCount(SoulsGathered);
		}
	}
}

void UAttributeComponent::UpdateTotalGold(int32 AmountOfGold)
{
	// Update internal state first, then HUD
	GoldAcquired = FMath::Max(0, GoldAcquired + AmountOfGold);

	if (UWorld* World = GetWorld())
	{
		if (AFillainPlayerController* FillainPlayerController = Cast<AFillainPlayerController>(World->GetFirstPlayerController()))
		{
			FillainPlayerController->SetHUDGoldCount(GoldAcquired);
		}
	}
}

void UAttributeComponent::CharactersReceiveMeleeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	AttributeSet->SetHealth(FMath::Clamp(AttributeSet->GetHealth() - DamageAmount, 0.f, AttributeSet->GetMaxHealth()));

	// Only proceed if owner is our expected character type
	if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(GetOwner()))
	{
		// DamageEvent.DamageTypeClass is a UClass*, not a UDamageType instance.
		// Get its CDO (class default object) or a default if none specified.
		const UDamageType* DamageType = nullptr;
		if (DamageEvent.DamageTypeClass)
		{
			DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
		}
		else
		{
			DamageType = GetDefault<UDamageType>();
		}

		// Pass a valid DamageType pointer to your receive function
		FillainCharacter->ReceiveDamage(GetOwner(), DamageAmount, DamageType, EventInstigator, DamageCauser);
	}
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	AttributeSet->SetStamina(FMath::Clamp(AttributeSet->GetStamina() - StaminaCost, 0.f, AttributeSet->GetMaxStamina()));
}