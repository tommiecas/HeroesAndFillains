// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFComponents/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

bool UAttributeComponent::IsCharacterAlive()
{
	return Health > 0.f;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

int32 UAttributeComponent::CharactersReceiveMeleeDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("🧪 Before: Health = %f, After = %f"), Health, FMath::Clamp(Health - Damage, 0.f, MaxHealth));
	return Health;
}

