#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseWidget.h"
#include "EnemyAttributeMenuWidget.generated.h"

/**
 * Enemy floating attribute widget (appears when hovering an enemy)
 */
UCLASS(Blueprintable, BlueprintType)
class HEROESANDFILLAINS_API UEnemyAttributeMenuWidget : public UEnemyBaseWidget
{
	GENERATED_BODY()

public:
	// Called when the Enemy assigns its controller
	virtual void SetWidgetController(UObject* InController) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowTemporarily(float Duration);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Trigger smooth fade in/out */
	void FadeIn(float Duration = 0.2f);
	void FadeOut(float Duration = 0.2f);

	virtual void WidgetControllerSet_Implementation() override;
	
protected:
	virtual void NativeConstruct() override;

	bool bIsFading = false;
	bool bFadeIn = true;
	float FadeTime = 0.f;
	float FadeDuration = 0.2f;
	
private:
	FTimerHandle FadeOutTimerHandle;
};
