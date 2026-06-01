// Copyright Alien Shores


#include "Actor/Significance/Factors/ViewportSignificanceFactor.h"

float UViewportSignificanceFactor::CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const
{
	const FVector ViewpointLocation = Viewpoint.GetLocation();
	const FVector CameraForward = Viewpoint.GetRotation().GetForwardVector();
	const FVector ActorLocation = ActorTransform.GetLocation();
	FVector DirectionToActor = (ActorLocation - ViewpointLocation);
	DirectionToActor.Normalize();
	const float DotToActor = FVector::DotProduct(CameraForward, DirectionToActor);
	if (DotToActor < CosHalfFOVThreshold)
	{
		// add a penalty based on angle outside of view
		float AngleAlpha = (DotToActor - (-1.f)) / (CosHalfFOVThreshold - (-1.f));
		AngleAlpha = FMath::Clamp(AngleAlpha, 0.f, 1.f);
		const float OffscreenAngleMultiplier = FMath::Pow(AngleAlpha, 2.f);
		return FMath::Lerp(0.1f, 0.9f, OffscreenAngleMultiplier);
	}
	// no penalty
	return 1.f;
}
