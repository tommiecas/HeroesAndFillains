#include "AI/HAFAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Enemies/EnemyBase.h"

AHAFAIController::AHAFAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void AHAFAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!InPawn) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (!Enemy || !Enemy->BehaviorTree) return;

	// ✅ Initialize the blackboard using the BehaviorTree's asset
	UBlackboardComponent* BBComp = Blackboard; // store local pointer

	if (!UseBlackboard(Enemy->BehaviorTree->BlackboardAsset, BBComp))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ❌ Failed to initialize blackboard"), *GetName());
		return;
	}

	// Optionally re-assign if UseBlackboard() replaced it
	Blackboard = BBComp;

	UE_LOG(LogTemp, Log, TEXT("[%s] ✅ Blackboard initialized successfully"), *GetName());

	UE_LOG(LogTemp, Log, TEXT("[%s] ✅ Blackboard initialized successfully"), *GetName());

	// ✅ Delay BehaviorTree start until ASC (and pawn) fully initialized
	TWeakObjectPtr<AEnemyBase> WeakEnemy = Enemy;

	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(
		DelayHandle,
		[this, WeakEnemy]()
		{
			if (!IsValid(this) || !WeakEnemy.IsValid()) return;

			AEnemyBase* LocalEnemy = WeakEnemy.Get();
			if (!LocalEnemy || !IsValid(LocalEnemy->GetController())) return;

			if (LocalEnemy->GetAbilitySystemComponent())
			{
				RunBehaviorTree(LocalEnemy->BehaviorTree);
				UE_LOG(LogTemp, Log, TEXT("[%s] ✅ BehaviorTree started safely"), *GetName());
			}
			else
			{
				FTimerHandle RetryHandle;
				GetWorldTimerManager().SetTimer(
					RetryHandle,
					[this, WeakEnemy]()
					{
						if (!IsValid(this) || !WeakEnemy.IsValid()) return;
						AEnemyBase* RetryEnemy = WeakEnemy.Get();
						if (!RetryEnemy || !IsValid(RetryEnemy->GetController())) return;

						if (RetryEnemy->GetAbilitySystemComponent())
						{
							RunBehaviorTree(RetryEnemy->BehaviorTree);
							UE_LOG(LogTemp, Log, TEXT("[%s] ✅ BehaviorTree started after retry"), *GetName());
						}
					},
					0.2f, false);
			}
		},
		0.25f, false);
}

void AHAFAIController::OnUnPossess()
{
	StopMovement();

	if (UPathFollowingComponent* PathComp = FindComponentByClass<UPathFollowingComponent>())
	{
		// 💥 Disable the tick function directly
		PathComp->PrimaryComponentTick.bCanEverTick = false;
		PathComp->SetComponentTickEnabled(false);
	}

	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}

	Super::OnUnPossess();
}
