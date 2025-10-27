#pragma once

#include "CoreMinimal.h"
// Other required includes go here
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeIhfo.generated.h"

UENUM(BlueprintType)
enum class EAttributeCategory : uint8
{
	Primary     UMETA(DisplayName = "Primary"),
	Secondary   UMETA(DisplayName = "Secondary"),
	Resistance  UMETA(DisplayName = "Resistance"),
	Vital		UMETA(DisplayName = "Vital")
};

USTRUCT (BlueprintType)
struct FHAFAttributeInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttributeValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsLeftSideAttribute = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAttributeCategory AttributeCategory = EAttributeCategory::Primary;
};


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class HEROESANDFILLAINS_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FHAFAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHAFAttributeInfo> AttributeInformation;
};
