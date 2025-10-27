#include "UI/Widgets/EnemyBaseWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

void UEnemyBaseWidget::SetWidgetController(UObject* InController)
{
	if (!InController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Tried to set null WidgetController"), *GetName());
		return;
	}

	WidgetController = InController;
	PropagateControllerToChildren();
	WidgetControllerSet(); // Calls _Implementation or BP override

	UE_LOG(LogTemp, Log, TEXT("[%s] WidgetController set to %s"),
		*GetName(), *InController->GetName());
}

void UEnemyBaseWidget::PropagateControllerToChildren()
{
	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Missing WidgetTree in PropagateControllerToChildren"), *GetName());
		return;
	}

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (UEnemyBaseWidget* Child = Cast<UEnemyBaseWidget>(Widget))
		{
			Child->SetWidgetController(WidgetController);
		}
	}
}

void UEnemyBaseWidget::WidgetControllerSet_Implementation()
{
	// Default: no-op. Blueprints can override to bind to Enemy attributes.
}
