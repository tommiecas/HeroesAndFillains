#include "UI/Widgets/HAFUserWidget.h"
#include "Interfaces/EnemyAttributeMenuWidgetControllerInterface.h"
#include "UI/WidgetControllers/HAFWidgetController.h"
#include "Enemies/EnemyBase.h"

void UHAFUserWidget::SetWidgetController(UObject* InWidgetController)
{
	if (!InWidgetController || bHasWidgetControllerAlreadySet)
                         	{
                         		return;
                         	}
                         
                         	WidgetController = InWidgetController;
                         
                         	// --- CASE 1: Implements Enemy Attribute Interface ---
                         	if (InWidgetController->GetClass()->ImplementsInterface(UEnemyAttributeMenuWidgetControllerInterface::StaticClass()))
                         	{
                         		IEnemyAttributeMenuWidgetControllerInterface* Interface = Cast<IEnemyAttributeMenuWidgetControllerInterface>(InWidgetController);
                         		if (Interface)
                         		{
                         			UE_LOG(LogTemp, Warning, TEXT("SetWidgetController called on %s, with controller %s"),
										*GetNameSafe(this), *GetNameSafe(InWidgetController));

                         			if (!InWidgetController)
                         			{
                         				UE_LOG(LogTemp, Error, TEXT("InWidgetController is nullptr!"));
                         				return;
                         			}

                         			UE_LOG(LogTemp, Warning, TEXT("Implements interface? %s"),
										 InWidgetController->Implements<UEnemyAttributeMenuWidgetControllerInterface>() ? TEXT("TRUE") : TEXT("FALSE"));
                         			Interface->Execute_BindCallbacksToDependencies(InWidgetController);
                         			Interface->Execute_BroadcastInitialEnemyValues(InWidgetController);
                         			bWidgetWorks = true;
                         			UE_LOG(LogTemp, Log, TEXT("%s successfully bound to EnemyAttributeMenuWidgetControllerInterface."), *GetName());
		}
	}

	// --- CASE 2: Regular Fillain / Player Widget Controller ---
	else if (UHAFWidgetController* WC = Cast<UHAFWidgetController>(InWidgetController))
	{
		HAFWidgetController = WC;
		bWidgetWorks = true;
		UE_LOG(LogTemp, Log, TEXT("%s successfully bound to UHAFWidgetController."), *GetName());
	}

	// --- CASE 3: Unknown ---
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Unknown widget controller type: %s"),
			*GetName(), *GetNameSafe(InWidgetController->GetClass()));
	}

	// Mark as initialized and trigger any BP setup
	bHasWidgetControllerAlreadySet = true;
	WidgetControllerSet();
}

void UHAFUserWidget::UpdateOwnerEnemy(AEnemyBase* NewEnemy)
{
	OwnerEnemy = NewEnemy;
}

void UHAFUserWidget::InitForEnemy(AEnemyBase* InEnemy)
{
	if (InEnemy)
	{
		OwnerEnemy = InEnemy;
	}
}

void UHAFUserWidget::WidgetControllerSet_Implementation()
{

}
