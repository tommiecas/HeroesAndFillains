#pragma once

#include "CoreMinimal.h"

#include "EnemyAttributeMenuWidgetControllerInterface.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UObject;

// This is the reflection wrapper
UINTERFACE(Blueprintable)
class HEROESANDFILLAINS_API UEnemyAttributeMenuWidgetControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for Enemy Attribute Menu Widget Controllers
 * Implement this in any class that serves as a controller for enemy attribute widgets.
 */
class HEROESANDFILLAINS_API IEnemyAttributeMenuWidgetControllerInterface
{
	GENERATED_BODY()

public:

	/** Returns the AbilitySystemComponent associated with this enemy */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy UI")
	UAbilitySystemComponent* GetEnemyASC() const;

	/** Sets the widget controller reference on the widget */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy UI")
	void SetWidgetController(UObject* InWidgetController);

	/** Returns the AttributeSet associated with this enemy */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy UI")
	UAttributeSet* GetEnemyAttributeSet() const;

	/** Called when the widget wants to force a UI refresh */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy UI")
	void BroadcastInitialEnemyValues();

	/** Called to bind the widget’s callbacks to the enemy’s attributes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy UI")
	void BindCallbacksToDependencies();
};
