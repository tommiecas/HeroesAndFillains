#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HAFUserWidget.generated.h"

class UHAFWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
class AEnemyBase; // ✅ forward declare instead of including

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API UHAFUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Assigns a controller object to this widget */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void SetWidgetController(UObject* InWidgetController);

	/** Updates the owning enemy reference (for hover UI, etc.) */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void UpdateOwnerEnemy(AEnemyBase* NewEnemy);

	/** Initializes the widget for a specific enemy instance */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void InitForEnemy(AEnemyBase* InEnemy);

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UObject> WidgetController = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UHAFWidgetController> HAFWidgetController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<AEnemyBase> OwnerEnemy = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	bool bWidgetWorks = false;

protected:
	/** Called after the widget controller is successfully set */
	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void WidgetControllerSet();
	virtual void WidgetControllerSet_Implementation();

	bool bHasWidgetControllerAlreadySet = false;

public:
	FORCEINLINE UObject* GetWidgetController() const { return WidgetController; }
	FORCEINLINE AEnemyBase* GetOwnerEnemy() const { return OwnerEnemy; }
};
