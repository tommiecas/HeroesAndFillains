#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/HAFUserWidget.h"
#include "EnemyBaseWidget.generated.h"

/**
 * Base class for all Enemy UI Widgets.
 * Provides automatic controller propagation to children.
 * The controller is typically the owning AEnemyBase actor.
 */
UCLASS()
class HEROESANDFILLAINS_API UEnemyBaseWidget : public UHAFUserWidget
{
	GENERATED_BODY()

public:
	/** Override: sets the owning controller (usually AEnemyBase). */
	virtual void SetWidgetController(UObject* InController) override;

protected:
	/** Called once controller is assigned — override in Blueprint to bind data. */
	virtual void WidgetControllerSet_Implementation() override;


	/** Propagates controller to all child widgets that also inherit from UEnemyBaseWidget. */
	void PropagateControllerToChildren();
};
