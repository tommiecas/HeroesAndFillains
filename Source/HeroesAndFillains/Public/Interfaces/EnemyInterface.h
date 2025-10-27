#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class HEROESANDFILLAINS_API IEnemyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void HighlightActor() = 0;

	UFUNCTION()
	virtual void UnHighlightActor() = 0;

	UFUNCTION()
	virtual void OnHoverStart() = 0;

	UFUNCTION()
	virtual void OnHoverEnd() = 0;
};
