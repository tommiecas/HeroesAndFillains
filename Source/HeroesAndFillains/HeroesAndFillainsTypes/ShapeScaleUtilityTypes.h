#pragma once
#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

namespace ShapeScaleUtils
{
	// Returns false if Shape is null or unsupported
	inline bool GetBaseline(const UShapeComponent* Shape, float& OutSphereR, FVector& OutBoxExtent, float& OutCapsuleR, float& OutCapsuleHH)
	{
		OutSphereR = 0.f; OutBoxExtent = FVector::ZeroVector; OutCapsuleR = 0.f; OutCapsuleHH = 0.f;

		if (auto Sphere = Cast<const USphereComponent>(Shape))
		{
			OutSphereR = Sphere->GetUnscaledSphereRadius();
			return true;
		}
		if (auto Box = Cast<const UBoxComponent>(Shape))
		{
			OutBoxExtent = Box->GetUnscaledBoxExtent();
			return true;
		}
		if (auto Cap = Cast<const UCapsuleComponent>(Shape))
		{
			OutCapsuleR  = Cap->GetUnscaledCapsuleRadius();
			OutCapsuleHH = Cap->GetUnscaledCapsuleHalfHeight();
			return true;
		}
		return false;
	}

	inline bool ApplyScaledSize(UShapeComponent* Shape, float Factor /* e.g., 1.f + IntuitionScale */)
	{
		if (!Shape || Factor <= 0.f) return false;

		if (auto Sphere = Cast<USphereComponent>(Shape))
		{
			const float Base = Sphere->GetUnscaledSphereRadius();
			Sphere->SetSphereRadius(Base * Factor, true);
			return true;
		}
		if (auto Box = Cast<UBoxComponent>(Shape))
		{
			const FVector Base = Box->GetUnscaledBoxExtent();
			Box->SetBoxExtent(Base * Factor, true);
			return true;
		}
		if (auto Cap = Cast<UCapsuleComponent>(Shape))
		{
			const float BaseR  = Cap->GetUnscaledCapsuleRadius();
			const float BaseHH = Cap->GetUnscaledCapsuleHalfHeight();
			Cap->SetCapsuleSize(BaseR * Factor, BaseHH * Factor, true);
			return true;
		}
		return false;
	}
}