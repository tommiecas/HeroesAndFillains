// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"

#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/FillainCharacter.h"
#include "Enemies/EnemyBase.h"
#include "HAFComponents/AttributeComponent.h"

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFillainCharacter* PlayerCharacter = Cast<AFillainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (OtherActor == PlayerCharacter)
	{
		// Get the soul value
		int32 CollectedSouls = this->GetSoulValue();

		// **Add to player's total**
		if (PlayerCharacter)
		{
			PlayerCharacter->AttributeComponent->UpdateTotalSouls(CollectedSouls);
		}

		// **Update the enemy’s SoulsGathered if necessary**
		if (OriginEnemy && OriginEnemy->AttributeComponent)
		{
			OriginEnemy->AttributeComponent->UpdateTotalSouls(CollectedSouls);
		}

		Destroy();
	}
}

int32 ASoul::GetSoulValue() const
{
	return SoulValue;
}

void ASoul::SetSoulValue(int32 Value)
{
	SoulValue = Value;
	// UE_LOG(LogTemp, Warning, TEXT("SoulValue set to %d"), SoulValue);
}
