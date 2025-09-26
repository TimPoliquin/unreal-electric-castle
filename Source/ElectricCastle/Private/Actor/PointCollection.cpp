// Copyright Alien Shores


#include "Actor/PointCollection.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;
	Point_0 = CreateDefaultSubobject<USceneComponent>("Point_0");
	Points.Add(Point_0);
	SetRootComponent(Point_0);
	Point_1 = CreateDefaultSubobject<USceneComponent>("Point_1");
	Points.Add(Point_1);
	Point_1->SetupAttachment(GetRootComponent());
	Point_2 = CreateDefaultSubobject<USceneComponent>("Point_2");
	Points.Add(Point_2);
	Point_2->SetupAttachment(GetRootComponent());
	Point_3 = CreateDefaultSubobject<USceneComponent>("Point_3");
	Points.Add(Point_3);
	Point_3->SetupAttachment(GetRootComponent());
	Point_4 = CreateDefaultSubobject<USceneComponent>("Point_4");
	Points.Add(Point_4);
	Point_4->SetupAttachment(GetRootComponent());
	Point_5 = CreateDefaultSubobject<USceneComponent>("Point_5");
	Points.Add(Point_5);
	Point_5->SetupAttachment(GetRootComponent());
	Point_6 = CreateDefaultSubobject<USceneComponent>("Point_6");
	Points.Add(Point_6);
	Point_6->SetupAttachment(GetRootComponent());
	Point_7 = CreateDefaultSubobject<USceneComponent>("Point_7");
	Points.Add(Point_7);
	Point_7->SetupAttachment(GetRootComponent());
	Point_8 = CreateDefaultSubobject<USceneComponent>("Point_8");
	Points.Add(Point_8);
	Point_8->SetupAttachment(GetRootComponent());
	Point_9 = CreateDefaultSubobject<USceneComponent>("Point_9");
	Points.Add(Point_9);
	Point_9->SetupAttachment(GetRootComponent());
	Point_10 = CreateDefaultSubobject<USceneComponent>("Point_10");
	Points.Add(Point_10);
	Point_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(
	const FVector& GroundLocation,
	const int32 NumPoints,
	const float YawOverride
) const
{
	checkf(Points.Num() >= NumPoints, TEXT("Attempted to access Points out of bounds"));
	TArray<USceneComponent*> ArrayCopy;
	for (USceneComponent* Point : Points)
	{
		if (ArrayCopy.Num() >= NumPoints)
		{
			return ArrayCopy;
		}
		if (Point != Point_0)
		{
			FVector ToPoint = Point->GetComponentLocation() - Point_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Point->SetWorldLocation(Point_0->GetComponentLocation() + ToPoint);
		}
		const FVector RaisedLocation = FVector(
			Point->GetComponentLocation().X,
			Point->GetComponentLocation().Y,
			Point->GetComponentLocation().Z + 500.f
		);
		const FVector LoweredLocation = FVector(
			Point->GetComponentLocation().X,
			Point->GetComponentLocation().Y,
			Point->GetComponentLocation().Z - 500.f
		);
		TArray<AActor*> IgnoreActors;
		UElectricCastleAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			TArray<AActor*>(),
			TArray<FName>(),
			GetActorLocation(),
			1500.f,
			IgnoreActors
		);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByProfile(
			HitResult,
			RaisedLocation,
			LoweredLocation,
			FName("BlockAll"),
			QueryParams
		);
		const FVector AdjustedLocation = FVector(
			Point->GetComponentLocation().X,
			Point->GetComponentLocation().Y,
			HitResult.ImpactPoint.Z
		);
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
