// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PointCollection.h"

#include "AbilitySystem/HAFAbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Point_0 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_0"));
	ImmutablePoints.Add(Point_0);
	SetRootComponent(Point_0);

	Point_1 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_1"));
	ImmutablePoints.Add(Point_1);
	Point_1->SetupAttachment(GetRootComponent());

	Point_2 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_2"));
	ImmutablePoints.Add(Point_2);
	Point_2->SetupAttachment(GetRootComponent());

	Point_3 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_3"));
	ImmutablePoints.Add(Point_3);
	Point_3->SetupAttachment(GetRootComponent());

	Point_4 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_4"));
	ImmutablePoints.Add(Point_4);
	Point_4->SetupAttachment(GetRootComponent());

	Point_5 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_5"));
	ImmutablePoints.Add(Point_5);
	Point_5->SetupAttachment(GetRootComponent());

	Point_6 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_6"));
	ImmutablePoints.Add(Point_6);
	Point_6->SetupAttachment(GetRootComponent());

	Point_7 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_7"));
	ImmutablePoints.Add(Point_7);
	Point_7->SetupAttachment(GetRootComponent());

	Point_8 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_8"));
	ImmutablePoints.Add(Point_8);
	Point_8->SetupAttachment(GetRootComponent());

	Point_9 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_9"));
	ImmutablePoints.Add(Point_9);
	Point_9->SetupAttachment(GetRootComponent());

	Point_10 = CreateDefaultSubobject<USceneComponent>(TEXT("Point_10"));
	ImmutablePoints.Add(Point_10);
	Point_10->SetupAttachment(GetRootComponent());

}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumberOfPoints,
	float YawOverride)
{
	checkf(ImmutablePoints.Num() >= NumberOfPoints, TEXT("Attempted to access Immutable Points out of bounds."));

	TArray<USceneComponent*> ArrayCopy;
	for (USceneComponent* Point : ImmutablePoints)
	{
		if (ArrayCopy.Num() >= NumberOfPoints) return ArrayCopy;

		if (Point != Point_0)
		{
			FVector ToPoint = Point->GetComponentLocation() - Point_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Point->SetWorldLocation(Point_0->GetComponentLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, Point->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, Point->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;

		TArray<AActor*> IgnoreActors;
		UHAFAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Point->SetWorldLocation(AdjustedLocation);

		Point->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		ArrayCopy.Add(Point);
	}
	return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


