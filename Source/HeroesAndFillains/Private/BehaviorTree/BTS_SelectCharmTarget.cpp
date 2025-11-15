// BTS_SelectCharmTarget.cpp
#include "AbilitySystem/BTS_SelectCharmTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/EnemyBase.h"

UBTS_SelectCharmTarget::UBTS_SelectCharmTarget()
{
	// bNotifyBecomeRelevant = false;
	// bNotifyTick = true;
	// Interval = 0.2f;
	// RandomDeviation = 0.05f;
}

void UBTS_SelectCharmTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// AAIController* AIC = OwnerComp.GetAIOwner();
	// if (!AIC) return;

	// AEnemyBase* SelfEnemy = Cast<AEnemyBase>(AIC->GetPawn());
	// if (!SelfEnemy) return;

	// UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	// if (!BB) return;

	// const bool bIsCharmed = BB->GetValueAsBool(TEXT("IsCharmed"));
	// if (!bIsCharmed) return;

	// End condition #1: player dead
	// AActor* PlayerActor = Cast<AActor>(BB->GetValueAsObject(TEXT("PlayerActor")));
	// const bool bPlayerDeadOrGone = (!IsValid(PlayerActor) || PlayerActor->IsActorBeingDestroyed());
	// Replace with your proper “is dead” check if you track health.

	// Find nearest other enemy with tag "Enemy"
	// AActor* Best = nullptr;
	// float BestDistSqr = TNumericLimits<float>::Max();

	// TArray<AActor*> Candidates;
	// UGameplayStatics::GetAllActorsWithTag(SelfEnemy->GetWorld(), FName("Enemy"), Candidates);

	// const FVector Me = SelfEnemy->GetActorLocation();
	// for (AActor* A : Candidates)
	// {
	// 	if (A == SelfEnemy) continue; // don’t target self
	// 	const float DistSqr = FVector::DistSquared(Me, A->GetActorLocation());
	// 	if (DistSqr <= SearchRadius * SearchRadius && DistSqr < BestDistSqr)
	// 	{
			// Optional: skip already dead actors here
	// 		Best = A;
	// 		BestDistSqr = DistSqr;
	//	}
	// }

	// const bool bNoTargetsLeft = (Best == nullptr);

	// if (bNoTargetsLeft || bPlayerDeadOrGone)
	// {
	//	SelfEnemy->BeginFlee();
	//	return;
	// }

	// BB->SetValueAsObject(TEXT("CharmTarget"), Best);
}
