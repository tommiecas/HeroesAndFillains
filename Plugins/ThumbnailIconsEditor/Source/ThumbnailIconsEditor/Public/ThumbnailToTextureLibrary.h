#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "ThumbnailToTextureLibrary.generated.h"

class UStaticMesh;
class UWorld;

UCLASS()
class THUMBNAILICONSEDITOR_API UThumbnailToTextureLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Note: WorldContextObject replaces the UWorld pin
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Thumbnails",
		  meta=(WorldContext="WorldContextObject", DisplayName="Create Icon From Static Mesh Capture"))
	static UTexture2D* CreateIconFromStaticMeshCapture(
		UStaticMesh* Mesh,
		UObject* WorldContextObject,
		int32 Size,
		const FString& PackagePath,
		const FString& AssetNameOverride,
		bool bTransparentBackground = true
	);
};
