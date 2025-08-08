// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFComponents/AttributeComponent.h"

#include "AttributeSet.h"
#include "AbilitySystem/HAFAttributeSet.h"
#include "Characters/FillainCharacter.h"
#include "HUD/FillainHUD.h"
#include "HUD/OverlayWidget.h"
#include "HUD/HUD/FillainStaminaWidget.h"
#include "PlayerController/FillainPlayerController.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SoulsGathered = 0;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);

}

void UAttributeComponent::UpdateTotalSouls(int32 NumberOfSouls)
{
	SoulsGathered += NumberOfSouls;  
	UE_LOG(LogTemp, Warning, TEXT("SoulsGathered is now: %d"), SoulsGathered);
    
	// Update HUD
	AFillainPlayerController* FillainPlayerController = Cast<AFillainPlayerController>(GetWorld()->GetFirstPlayerController());
	if (FillainPlayerController)
	{
		FillainPlayerController->SetHUDSoulsCount(SoulsGathered);
	}
}

void UAttributeComponent::UpdateTotalGold(int32 AmountOfGold)
{
	AFillainPlayerController* FillainPlayerController = Cast<AFillainPlayerController>(GetWorld()->GetFirstPlayerController());
	if (FillainPlayerController)
	{
		FillainPlayerController->SetHUDGoldCount(GoldAcquired += AmountOfGold);
	}
}


void UAttributeComponent::CharactersReceiveMeleeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	AFillainCharacter* Fillain = Cast<AFillainCharacter>(GetOwner());
	UHAFAttributeSet* Attributes= Cast<UHAFAttributeSet>(Fillain->GetHAFAttributeSet());
	Attributes->Health = FMath::Clamp(GetHealth() - DamageAmount, 0.f, Attributes->GetMaxHealth());
	if (this->GetOwner()->IsA(AFillainCharacter::StaticClass()))
	{
		if (AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(this->GetOwner()))
		{
			UDamageType* DamageType = Cast<UDamageType>(DamageEvent.DamageTypeClass);
			FillainCharacter->ReceiveDamage(GetOwner(), DamageAmount, DamageType, EventInstigator, DamageCauser);
		}
	}
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
	AFillainHUD* FillainHUD = Cast<AFillainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (FillainHUD &&
		FillainHUD->OverlayWidget &&
		FillainHUD->OverlayWidget->FillainStaminaWidget &&
		FillainHUD->OverlayWidget->FillainStaminaWidget->StaminaProgressBar)
	{
		FillainHUD->OverlayWidget->FillainStaminaWidget->UpdateStaminaBar(Stamina);
	}
}