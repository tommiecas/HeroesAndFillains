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
#include "UObject/SavePackage.h"

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
    bool bTransparentBackground // if true: transparent black; else: solid black
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

    // --- Render target (black/transparent background, sRGB) ---
    UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(GetTransientPackage());
    RT->TargetGamma = 2.2f;
    RT->InitCustomFormat(Size, Size, PF_B8G8R8A8, /*bForceLinearGamma*/ true);
    RT->ClearColor = bTransparentBackground ? FLinearColor(0,0,0,0) : FLinearColor::Black;
    RT->UpdateResourceImmediate();

    // --- Transient stage + subject mesh ---
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

    // Lift slightly ABOVE Z=0 to avoid any thin floor line
    const FBoxSphereBounds B0 = SMC->CalcBounds(SMC->GetComponentTransform());
    const float ExtraLift = B0.BoxExtent.Z * 0.05f; // +5% height
    SMC->AddLocalOffset(FVector(0, 0, -B0.Origin.Z + B0.BoxExtent.Z + ExtraLift));

    // Recompute centered bounds after moving
    const FBoxSphereBounds B = SMC->CalcBounds(SMC->GetComponentTransform());
    const FVector Center = B.Origin;
    const float   Radius = FMath::Max(1.f, B.SphereRadius);

    // --- Lighting: Key + Fill (balanced midtones) ---
    UDirectionalLightComponent* Key = NewObject<UDirectionalLightComponent>(Stage);
    Key->SetupAttachment(SceneRoot);
    Key->SetMobility(EComponentMobility::Movable);
    Key->Intensity = 1800.f;  // tweak 1400–2400 to taste
    Key->LightColor = FColor::White;
    Key->CastShadows = true;
    Key->SetWorldRotation(FRotator(-35.f, 35.f, 0.f));
    Key->RegisterComponent();

    UDirectionalLightComponent* Fill = NewObject<UDirectionalLightComponent>(Stage);
    Fill->SetupAttachment(SceneRoot);
    Fill->SetMobility(EComponentMobility::Movable);
    Fill->Intensity = 700.f;  // softer than key
    Fill->LightColor = FColor::White;
    Fill->CastShadows = false;
    Fill->SetWorldRotation(FRotator(-15.f, -30.f, 0.f));
    Fill->RegisterComponent();

    // --- Scene capture (orthographic, centered, show-only) ---
    ASceneCapture2D* Cap = World->SpawnActor<ASceneCapture2D>(SP);
    if (!Cap) { Stage->Destroy(); return nullptr; }
    USceneCaptureComponent2D* Cap2D = Cap->GetCaptureComponent2D();

    Cap2D->ProjectionType = ECameraProjectionMode::Orthographic;
    const float Padding = 1.18f;                  // ~18% empty border
    Cap2D->OrthoWidth = (Radius * 2.f) * Padding;

    const float ViewDist = Radius * 4.0f;         // distance irrelevant in ortho; keep > 0
    const FVector CamLoc  = Center + FVector(ViewDist, 0, 0);
    Cap->SetActorLocation(CamLoc);
    Cap->SetActorRotation((Center - CamLoc).Rotation());

    Cap2D->TextureTarget = RT;
    Cap2D->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

    // Only render our staged mesh; ignore the rest of the map
    Cap2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Cap2D->ShowOnlyComponent(SMC);

    // Lock exposure (no swings); remove bloom/vignette; neutral tonemapper
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

    // --- Capture → readback ---
    Cap2D->bCaptureEveryFrame = false;
    Cap2D->bCaptureOnMovement = false;
    Cap2D->CaptureScene();
    FlushRenderingCommands();

    TArray<FColor> SurfData;
    FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
    if (!Res || !Res->ReadPixels(SurfData))
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: ReadPixels failed."));
        if (Cap)   Cap->Destroy();
        if (Stage) Stage->Destroy();
        return nullptr;
    }

    const int32 NumPixels = Size * Size;
    if (SurfData.Num() != NumPixels)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateIcon: Unexpected pixel count %d (expected %d)."), SurfData.Num(), NumPixels);
        if (Cap)   Cap->Destroy();
        if (Stage) Stage->Destroy();
        return nullptr;
    }

    // --- Create Texture2D asset (PERSISTENT) ---
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

    // UI-friendly defaults
    Tex->NeverStream = true;
    Tex->VirtualTextureStreaming = false;
    Tex->SRGB = true;
    Tex->CompressionSettings = TC_EditorIcon;
    Tex->MipGenSettings = TMGS_NoMipmaps;
    Tex->LODGroup = TEXTUREGROUP_UI;

    // Persist pixels in Source (critical for surviving reopen/cook)
    Tex->Source.Init(
        Size, Size,
        /*NumSlices*/ 1,
        /*NumMips*/   1,
        TSF_BGRA8,
        reinterpret_cast<const uint8*>(SurfData.GetData())
    );

    // Fill PlatformData so it shows immediately in the editor
    Tex->SetPlatformData(new FTexturePlatformData());
    Tex->GetPlatformData()->SizeX = Size;
    Tex->GetPlatformData()->SizeY = Size;
    Tex->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

    FTexture2DMipMap* Mip = new FTexture2DMipMap();
    Mip->SizeX = Size;
    Mip->SizeY = Size;

    const SIZE_T Bytes = SIZE_T(NumPixels) * sizeof(FColor);
    void* Dest = Mip->BulkData.Lock(LOCK_READ_WRITE);
    Dest = Mip->BulkData.Realloc(Bytes);                  // IMPORTANT: allocate before copying
    FMemory::Memcpy(Dest, SurfData.GetData(), Bytes);
    Mip->BulkData.Unlock();

    Tex->GetPlatformData()->Mips.Add(Mip);

    // Update resource & notify editor/asset registry
    Tex->UpdateResource();
    Tex->PostEditChange();

    FAssetRegistryModule::AssetCreated(Tex);
    Tex->MarkPackageDirty();

    // (Optional) Save to disk immediately:
    
         const FString FilePath = FPackageName::LongPackageNameToFilename(
             UniquePkg, FPackageName::GetAssetPackageExtension());
         FSavePackageArgs Args;
         Args.TopLevelFlags = RF_Public | RF_Standalone;
         Args.SaveFlags = SAVE_None;
         UPackage::SavePackage(Pkg, Tex, *FilePath, Args);
    

    // --- Cleanup ---
    if (Cap)   Cap->Destroy();
    if (Stage) Stage->Destroy();

    UE_LOG(LogTemp, Display, TEXT("CreateIcon: Created %s at %s"), *UniqueName, *UniquePkg);
    return Tex;
#else
    return nullptr;
#endif
}
