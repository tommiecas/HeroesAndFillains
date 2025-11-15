// BTTask_ChainFleeHop.cpp
#include "BehaviorTree/BTTask_ChainFleeHop.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

EBTNodeResult::Type UBTTask_ChainFleeHop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
//	if (AAIController* AIC = OwnerComp.GetAIOwner())
//	{
//		if (AEnemyBase* E = Cast<AEnemyBase>(AIC->GetPawn()))
//		{
//			E->DoNextFleeHop(); // sets next RunDestination & MoveTo
//			return EBTNodeResult::Succeeded;
//		}
//	}
	return EBTNodeResult::Failed;
}
