// Copyright Alien Shores


#include "Actor/Significance/Factors/DistanceSignificanceFactor.h"

float UDistanceSignificanceFactor::CalculateSignificance(const AActor* ActorToEvaluate, const FTransform& ActorTransform, const FTransform& Viewpoint) const
{
	const FVector ViewpointLocation = Viewpoint.GetLocation();
	const FVector ActorLocation = ActorTransform.GetLocation();
	const float Distance = FVector::Distance(ViewpointLocation, ActorLocation);

	// Hard Cull Range: Even if they are alerted, if they are past this extreme distance, 
	// they don't matter to our direct visual/budget system.
	if (Distance <= FullSignificanceRange)
	{
		return 1.f;
	}
	if (Distance >= InsignificantRange)
	{
		return 0.f;
	}
	// Baseline linear range fallback (1.0 at 0cm, 0.0 at MaxRange)
	const float DistanceSignificance = 1.f - (Distance / (InsignificantRange - FullSignificanceRange));
	return DistanceSignificance;
}
