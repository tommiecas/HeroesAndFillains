// Source/ThumbnailIconsEditor/Private/ThumbnailToTextureLibrary.cpp

#include "ThumbnailToTextureLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureDefines.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"

// Resolve an Editor World from any UObject context (works from an EUW when you pass 'Self')
static UWorld* ResolveEditorWorld(UObject* WorldContextObject)
{
#if WITH_EDITOR
    if (!GEngine) return nullptr;

    if (UWorld* W = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
    {
        return W;
    }
    for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
    {
        if (Ctx.WorldType == EWorldType::Editor || Ctx.WorldType == EWorldType::EditorPreview)
        {
            return Ctx.World();
        }
    }
#endif
    return nullptr;
}

UTexture2D* UThumbnailToTextureLibrary::CreateIconFromStaticMeshCapture(
    UStaticMesh* Mesh,
    UObject* WorldContextObject,
    int32 Size,
    const FString& PackagePath,
    const FString& AssetNameOverride,
    bool /*bTransparentBackground*/
)
{
#if WITH_EDITOR
    if (!Mesh || Size <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: Invalid Mesh or Size."));
        return nullptr;
    }

    UWorld* World = ResolveEditorWorld(WorldContextObject);
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: Could not resolve an Editor UWorld. Run from an EUW and pass Self."));
        return nullptr;
    }

    // Normalize package path + name
    const FString SanitizedPath = PackagePath.StartsWith(TEXT("/")) ? PackagePath : TEXT("/") + PackagePath;
    const FString BaseName = AssetNameOverride.IsEmpty() ? (Mesh->GetName() + TEXT("_Icon")) : AssetNameOverride;

    // 1) Render target (black background, sRGB)
    UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(GetTransientPackage());
    RT->TargetGamma = 2.2f;
    RT->InitCustomFormat(Size, Size, PF_B8G8R8A8, /*bForceLinearGamma*/ true);
    RT->ClearColor = FLinearColor::Black;
    RT->UpdateResourceImmediate();

    // 2) Transient stage
    FActorSpawnParameters SP; SP.ObjectFlags |= RF_Transient;

    AActor* Stage = World->SpawnActor<AActor>(SP);
    if (!Stage) return nullptr;

    USceneComponent* SceneRoot = NewObject<USceneComponent>(Stage);
    Stage->SetRootComponent(SceneRoot);
    SceneRoot->RegisterComponent();

    UStaticMeshComponent* SMC = NewObject<UStaticMeshComponent>(Stage);
    SMC->SetupAttachment(SceneRoot);
    SMC->SetStaticMesh(Mesh);
    SMC->SetMobility(EComponentMobility::Movable);
    SMC->SetCastShadow(true);
    SMC->RegisterComponent();
    
    // Pleasant default angle
    SMC->AddLocalRotation(FRotator(0.f, 20.f, 0.f));

    // Lift so the mesh sits slightly ABOVE Z=0 to avoid any "floor line"
    const FBoxSphereBounds B0 = SMC->CalcBounds(SMC->GetComponentTransform());
    const float ExtraLift = B0.BoxExtent.Z * 0.05f; // +5% height
    SMC->AddLocalOffset(FVector(0, 0, -B0.Origin.Z + B0.BoxExtent.Z + ExtraLift));

    // Recompute centered bounds after moving
    const FBoxSphereBounds B = SMC->CalcBounds(SMC->GetComponentTransform());
    const FVector Center = B.Origin;
    const float   Radius = FMath::Max(1.f, B.SphereRadius);
    
    // 3) Lighting — Key + Fill (balanced)
    UDirectionalLightComponent* Key = NewObject<UDirectionalLightComponent>(Stage);
    Key->SetupAttachment(SceneRoot);
    Key->SetMobility(EComponentMobility::Movable);
    Key->Intensity = 1800.f;             // tweak 1400–2400 as needed
    Key->LightColor = FColor::White;
    Key->CastShadows = true;
    Key->SetWorldRotation(FRotator(-35.f, 35.f, 0.f));
    Key->RegisterComponent();

    UDirectionalLightComponent* Fill = NewObject<UDirectionalLightComponent>(Stage);
    Fill->SetupAttachment(SceneRoot);
    Fill->SetMobility(EComponentMobility::Movable);
    Fill->Intensity = 700.f;             // softer fill
    Fill->LightColor = FColor::White;
    Fill->CastShadows = false;
    Fill->SetWorldRotation(FRotator(-15.f, -30.f, 0.f));
    Fill->RegisterComponent();

    // 4) Scene capture — orthographic for guaranteed framing
    ASceneCapture2D* Cap = World->SpawnActor<ASceneCapture2D>(SP);
    if (!Cap) { Stage->Destroy(); return nullptr; }
    USceneCaptureComponent2D* Cap2D = Cap->GetCaptureComponent2D();

    // Get the mesh bounds in local space
    FVector Origin;
    FVector BoxExtent;
    SMC->GetLocalBounds(Origin, BoxExtent);

    // Convert to world space for positioning
    FVector WorldOrigin = SMC->GetComponentLocation() + Origin;

    // Aim the capture at the center of the mesh
   //  Cap->SetActorLocation(WorldOrigin + FVector(-BoxExtent.X * 2.5f, 0, 0)); // Pull camera back
    // Cap->SetActorRotation((WorldOrigin - Cap->GetActorLocation()).Rotation());
    
    // Only render our staged mesh; ignore the rest of the map
    Cap2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Cap2D->ShowOnlyComponent(SMC);
    // (Optional) if you ever add more primitives (e.g., a custom floor), ShowOnlyComponent() them too.
    
    // === PERFECT CENTERING (ortho) ===
    // After you compute:  Center (B.Origin) and Radius

    // 1) Ortho framing with a little padding
    Cap2D->ProjectionType = ECameraProjectionMode::Orthographic;
    const float Padding = 1.18f;                     // ~18% empty border
    Cap2D->OrthoWidth = (Radius * 2.f) * Padding;

    // 2) Show only our mesh (already added earlier, keep it!)
    Cap2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Cap2D->ShowOnlyComponent(SMC);

    // 3) Put the camera exactly on the +X axis at the SAME Z as the bounds center
    const float ViewDist = Radius * 4.0f;            // any positive distance is fine in ortho
    const FVector CamLoc  = Center + FVector(ViewDist, 0, 0);
    Cap->SetActorLocation(CamLoc);

    // 4) Look straight at the bounds center (no extra tilt)
    Cap->SetActorRotation((Center - CamLoc).Rotation());
    // === END CENTERING ===
    
    Cap2D->TextureTarget = RT;
    Cap2D->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

    // Lock exposure; remove bloom/vignette; neutral tonemapper
    Cap2D->PostProcessSettings.bOverride_AutoExposureMethod = true;
    Cap2D->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
    Cap2D->PostProcessSettings.bOverride_AutoExposureBias = true;
    Cap2D->PostProcessSettings.AutoExposureBias = 0.0f;
    Cap2D->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
    Cap2D->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
    Cap2D->PostProcessSettings.AutoExposureMinBrightness = 1.0f;
    Cap2D->PostProcessSettings.AutoExposureMaxBrightness = 1.0f;
    Cap2D->PostProcessSettings.bOverride_BloomIntensity = true;
    Cap2D->PostProcessSettings.BloomIntensity = 0.0f;
    Cap2D->PostProcessSettings.bOverride_VignetteIntensity = true;
    Cap2D->PostProcessSettings.VignetteIntensity = 0.0f;

    Cap2D->ShowFlags.SetSkyLighting(false);
    Cap2D->ShowFlags.SetAtmosphere(false);
    Cap2D->ShowFlags.SetFog(false);
    Cap2D->ShowFlags.SetTonemapper(true);

    // 5) Capture → readback
    Cap2D->bCaptureEveryFrame = false;
    Cap2D->bCaptureOnMovement = false;
    Cap2D->CaptureScene();
    FlushRenderingCommands();

    TArray<FColor> SurfData;
    FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
    if (!Res || !Res->ReadPixels(SurfData) || SurfData.Num() != Size * Size)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: ReadPixels failed or unexpected size."));
        if (Cap)   Cap->Destroy();
        if (Stage) Stage->Destroy();
        return nullptr;
    }

    // 6) Create Texture2D asset
    FAssetToolsModule& ATM = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    FString UniquePkg, UniqueName;
    ATM.Get().CreateUniqueAssetName(SanitizedPath / BaseName, TEXT(""), UniquePkg, UniqueName);

    UPackage* Pkg = CreatePackage(*UniquePkg);
    if (!Pkg)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: Failed to create package %s"), *UniquePkg);
        if (Cap)   Cap->Destroy();
        if (Stage) Stage->Destroy();
        return nullptr;
    }

    UTexture2D* Tex = NewObject<UTexture2D>(Pkg, *UniqueName, RF_Public | RF_Standalone | RF_Transactional);
    Tex->SetPlatformData(new FTexturePlatformData());
    Tex->GetPlatformData()->SizeX = Size;
    Tex->GetPlatformData()->SizeY = Size;
    Tex->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

    FTexture2DMipMap* Mip = new FTexture2DMipMap();
    Mip->SizeX = Size;
    Mip->SizeY = Size;
    Mip->BulkData.Lock(LOCK_READ_WRITE);
    void* Dest = Mip->BulkData.Realloc(Size * Size * sizeof(FColor));
    FMemory::Memcpy(Dest, SurfData.GetData(), Size * Size * sizeof(FColor));
    Mip->BulkData.Unlock();
    Tex->GetPlatformData()->Mips.Add(Mip);

    Tex->SRGB = true;
    Tex->CompressionSettings = TC_EditorIcon;
    Tex->MipGenSettings = TMGS_NoMipmaps;
    Tex->UpdateResource();

    FAssetRegistryModule::AssetCreated(Tex);
    Tex->MarkPackageDirty();

    // 7) Cleanup
    if (Cap)   Cap->Destroy();
    if (Stage) Stage->Destroy();

    UE_LOG(LogTemp, Display, TEXT("CreateIcon: Created %s at %s"), *UniqueName, *UniquePkg);
    return Tex;
#else
    return nullptr;
#endif
}
