#include "HAFComponents/HiddenTreasureComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UHiddenTreasureComponent::UHiddenTreasureComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // smooth fade
}

UMeshComponent* UHiddenTreasureComponent::FindBestMesh() const
{
	// 1) If designer specified one explicitly, use it.
	if (IsValid(ExplicitHighlightMesh))
	{
		return ExplicitHighlightMesh;
	}

	// 2) Gather all mesh components on the owner.
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	TInlineComponentArray<UMeshComponent*> Meshes(Owner);
	if (Meshes.Num() == 0) return nullptr;

	// 3) If any mesh has the preferred tag, pick the first one.
	for (UMeshComponent* M : Meshes)
	{
		if (IsValid(M) && M->ComponentHasTag(PreferredHighlightTag))
		{
			return M;
		}
	}

	// 4) Otherwise, pick the *largest visible* mesh (best for props with multiple parts).
	UMeshComponent* Best = nullptr;
	float BestRadius = -1.f;
	for (UMeshComponent* M : Meshes)
	{
		if (!IsValid(M) || !M->IsVisible()) continue;
		const float Radius = M->CalcBounds(M->GetComponentTransform()).SphereRadius;
		if (Radius > BestRadius)
		{
			BestRadius = Radius;
			Best = M;
		}
	}

	// 5) Fallback: first mesh if none were visible (e.g., will be shown later).
	return Best ? Best : Meshes[0];
}

void UHiddenTreasureComponent::BeginPlay()
{
	Super::BeginPlay();
	BuildMIDs();
}

void UHiddenTreasureComponent::BuildMIDs()
{
	if (bBuilt) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	TInlineComponentArray<UMeshComponent*> Meshes(Owner);
	for (UMeshComponent* Mesh : Meshes)
	{
		const int32 Num = Mesh->GetNumMaterials();
		for (int32 i = 0; i < Num; ++i)
		{
			if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i))
			{
				MID->SetScalarParameterValue(GlowParamName, MinGlow);
				GlowMIDs.Add(MID);
			}
		}
	}
	bBuilt = GlowMIDs.Num() > 0;
	UE_LOG(LogTemp, Warning, TEXT("[HT] Built %d MIDs on %s"), GlowMIDs.Num(), *GetNameSafe(GetOwner()));
}

void UHiddenTreasureComponent::ApplyIntuitionScale(float Fraction)
{
	const bool bGlow = Fraction > 0.05f;

	if (UMeshComponent* Mesh = FindBestMesh())
	{
		// Outline
		Mesh->SetRenderCustomDepth(bGlow);
		Mesh->SetCustomDepthStencilValue(1); // match your post-process

		// Emissive (assuming your material has a scalar param named "Glow")
		for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
		{
			if (UMaterialInstanceDynamic* MID = Mesh->CreateAndSetMaterialInstanceDynamic(i))
			{
				MID->SetScalarParameterValue(TEXT("Glow"), Fraction);
			}
		}
	}

	// ❌ Do NOT apply gameplay here (no OnOverlap / no ASC calls)
}

void UHiddenTreasureComponent::TickComponent(float DeltaTime, ELevelTick, FActorComponentTickFunction*)
{
	if (!bBuilt || GlowMIDs.Num() == 0) return;

	CurrentGlow = FMath::FInterpTo(CurrentGlow, TargetGlow, DeltaTime, FadeSpeed);
	for (UMaterialInstanceDynamic* MID : GlowMIDs)
	{
		if (MID) MID->SetScalarParameterValue(GlowParamName, CurrentGlow);
	}
}