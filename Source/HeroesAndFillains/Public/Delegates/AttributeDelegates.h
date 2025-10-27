#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"       // ✅ ensures UHT parses this header
#include "AttributeDelegates.generated.h"

// Forward declare to avoid circular include
struct FHAFAttributeInfo;

// Dummy USTRUCT just so UHT will generate this header properly
USTRUCT()
struct FAttributeDelegatesDummy
{
	GENERATED_BODY()
};

// ✅ Now declare your delegates (UHT will see them)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FHAFAttributeInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChanged, float, NewValue);