#pragma once

#include "CoreMinimal.h"
#include "Delegates/AttributeDelegates.h"
#include "UObject/NoExportTypes.h"
#include "EnemyWidgetControllerBase.generated.h"

class UHAFAttributeSet;
struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;
class FAttributeInfoSignature;
class AEnemyBase;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyAttributeInfoSignature, const FHAFAttributeInfo&, Info);

UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API UEnemyWidgetControllerBase : public UObject
{
	GENERATED_BODY()

public:
	/** Initialize the controller with its owning enemy */
	UFUNCTION(BlueprintCallable, Category = "Enemy Widget Controller")
	void Initialize(AEnemyBase* InEnemy);

	UPROPERTY(BlueprintAssignable, Category = "Enemy Widget Controller")
	FEnemyAttributeInfoSignature EnemyAttributeInfoDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Enemy Widget Controller")
	void AssignEnemyAttributeInfoDelegate();
	
	/** Expose to Blueprints so widgets can query these */
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	AEnemyBase* Enemy = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	UAbilitySystemComponent* EnemyASC = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	const UHAFAttributeSet* EnemyHAFAttributeSet = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAttributeInfo* EnemyAttributeInfo;

	
	/** For BP binding or C++ delegate registration */
	// UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Widget Controller")
	UFUNCTION(BlueprintCallable, Category = "Enemy Widget Controller")
	void BindCallbacksToDependencies();

	// UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Widget Controller")
	UFUNCTION(BlueprintCallable, Category = "Enemy Widget Controller")
	void BroadcastInitialEnemyValues();

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastEnemyAttributeInfo(const FGameplayTag& EnemyAttributeTag, const FGameplayAttribute& EnemyAttribute) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetEnemyAttributeInfo(UAttributeInfo* InEnemyAttributeInfo) { EnemyAttributeInfo = InEnemyAttributeInfo; }

	UPROPERTY()
	AEnemyBase* OwningEnemy = nullptr;
	
	FORCEINLINE AEnemyBase* GetOwningEnemy() const { return OwningEnemy; }
};
