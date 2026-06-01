// Copyright Alien Shores


#include "Actor/Track/TrackFunctionLibrary.h"


#include "Components/SplineComponent.h"

FVector UTrackFunctionLibrary::GetLocationOnTrack(const USplineComponent* Spline, const float Progress)
{
	return Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);
}

FRotator UTrackFunctionLibrary::GetRotationOnTrack(const USplineComponent* Spline, const float Progress)
{
	return Spline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);
}

bool UTrackFunctionLibrary::HasMoreTrack(const USplineComponent* Spline, const float Progress)
{
	return Spline->GetSplineLength() > Progress;
}

float UTrackFunctionLibrary::GetClosestDistanceOnTrack(const USplineComponent* Spline, const FVector& Location)
{
	return Spline->GetDistanceAlongSplineAtLocation(Location, ESplineCoordinateSpace::World);
}
