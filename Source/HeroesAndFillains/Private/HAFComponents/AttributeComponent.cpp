#include "HAFComponents/AttributeComponent.h"

#include "Characters/FillainCharacter.h"
#include "HUD/FillainHUD.h"
#include "PlayerController/FillainPlayerController.h"
#include "GameFramework/DamageType.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	FillAttSet = nullptr; 
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
	if (AFillainCharacter* Fillain = Cast<AFillainCharacter>(GetOwner()))
	{
		FillAttSet = Cast<UHAFAttributeSet>(Fillain->GetAttributeSet());
		FillAttSet->SetStamina(FMath::Clamp(FillAttSet->GetStamina() + StaminaRegenRate * DeltaTime, 0.f, FillAttSet->GetMaxStamina()));		
	}

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
	FillAttSet->SetHealth(FMath::Clamp(FillAttSet->GetHealth() - DamageAmount, 0.f, FillAttSet->GetMaxHealth()));

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
	if (ABaseCharacter* Char = Cast<ABaseCharacter>(GetOwner()))
	{
		if (!Char->AbilitySystemComponent || !Char->HAFAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("UseStamina: ASC or AttributeSet not ready"));
			return;
		}

		const float MaxSta = Char->GetHAFAttributeSet()->GetMaxStamina(); // inline getter is fine now
		const float CurSta = Char->GetHAFAttributeSet()->GetStamina();

		if (!FMath::IsFinite(MaxSta) || !FMath::IsFinite(CurSta))
		{
			UE_LOG(LogTemp, Error, TEXT("UseStamina: Non-finite values (Cur:%f Max:%f)"), CurSta, MaxSta);
			return;
		}

		if (CurSta < StaminaCost) return;

		// Apply a GE to modify Stamina, or write directly if you’re not using GEs:
		// Avoid direct writes if you’re in full GAS; prefer gameplay effects.
		// ...
		
		FillAttSet->SetStamina(FMath::Clamp(FillAttSet->GetStamina() - StaminaCost, 0.f, FillAttSet->GetMaxStamina()));
	}
}