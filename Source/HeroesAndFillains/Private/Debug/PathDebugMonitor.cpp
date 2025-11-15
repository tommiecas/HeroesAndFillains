#include "Debug/PathDebugMonitor.h"
#include "Navigation/PathFollowingComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"

APathDebugMonitor::APathDebugMonitor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APathDebugMonitor::BeginPlay()
{
	Super::BeginPlay();

	// Start repeating timer using *persistent* handle
	GetWorldTimerManager().SetTimer(
		PathTickMonitorHandle,
		this,
		&APathDebugMonitor::CheckPathComponents,
		0.5f,     // checks twice per second so we see the PF before the crash
		true
	);
}

void APathDebugMonitor::CheckPathComponents()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;

		UPathFollowingComponent* PF = Actor->FindComponentByClass<UPathFollowingComponent>();
		if (PF)
		{
			AController* AsController = Cast<AController>(Actor);
			APawn* Pawn = AsController ? AsController->GetPawn() : nullptr;

			UE_LOG(LogTemp, Error, TEXT("PF ACTIVE -> Owner=%s | Controller=%s | Pawn=%s | Active=%d | Tick=%d"),
				*GetNameSafe(Actor),
				*GetNameSafe(AsController),
				*GetNameSafe(Pawn),
				PF->IsActive(),
				PF->IsComponentTickEnabled()
			);
		}
	}
}
