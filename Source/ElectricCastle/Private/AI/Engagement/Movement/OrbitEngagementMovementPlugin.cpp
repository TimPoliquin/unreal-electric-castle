// Copyright Alien Shores


#include "AI/Engagement/Movement/OrbitEngagementMovementPlugin.h"

#include "NavigationSystem.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Utils/RandUtils.h"

void UOrbitEngagementMovementPlugin::TickMovement_Implementation(const float DeltaTime)
{
	if (!TargetActor.IsValid())
	{
		return;
	}
	if (!GetMovementComponent())
	{
		return;
	}
	const FVector& DesiredVelocity = CalculateDesiredVelocity(DeltaTime);
	if (bUseNavSystem && IsDesiredLocationNavigable(OwnerActor->GetActorLocation() + DesiredVelocity * DeltaTime))
	{
		GetMovementComponent()->AddInputVector(DesiredVelocity);
	}
	else
	{
		ChangeItUp();
		GetMovementComponent()->AddInputVector(DesiredVelocity);
	}
}

void UOrbitEngagementMovementPlugin::ChangeItUp_Implementation()
{
	Super::ChangeItUp_Implementation();
	bOrbitClockwise = URandRangeBlueprintLibrary::ShouldAct(.5f);
	OrbitSpeed = URandRangeBlueprintLibrary::GetRandomFloatInRange(OrbitSpeedRange);
}

void UOrbitEngagementMovementPlugin::SetTargetActor_Implementation(AActor* InTargetActor)
{
	Super::SetTargetActor_Implementation(InTargetActor);
	bOrbitClockwise = URandRangeBlueprintLibrary::ShouldAct(.5f);
	OrbitSpeed = URandRangeBlueprintLibrary::GetRandomFloatInRange(OrbitSpeedRange);
}

FVector UOrbitEngagementMovementPlugin::CalculateSmoothOrbitVelocity(const FVector& RadialDirection, const float DeltaTime)
{
	// --- Compute target tangent ---
	FVector TargetTangent = FVector::CrossProduct(RadialDirection, FVector::UpVector);
	if (bOrbitClockwise)
	{
		TargetTangent *= -1.f;
	}
	TargetTangent.Normalize();

	// --- Smooth tangent ---
	if (SmoothedTangentDirection.IsNearlyZero())
	{
		SmoothedTangentDirection = TargetTangent;
	}
	else
	{
		SmoothedTangentDirection = FMath::VInterpTo(
			SmoothedTangentDirection,
			TargetTangent,
			DeltaTime,
			TangentSmoothingSpeed
		);
	}
	return SmoothedTangentDirection * OrbitSpeed;
}

FVector UOrbitEngagementMovementPlugin::CalculateDesiredVelocity(const float DeltaTime)
{
	if (FVector DirectionToTarget; GetDirectionToTarget(DirectionToTarget))
	{
		const float Distance = DirectionToTarget.Length();
		const FVector RadialDirection = DirectionToTarget / Distance;
		FVector TangentialDirection = FVector::CrossProduct(RadialDirection, FVector::UpVector);
		if (bOrbitClockwise)
		{
			TangentialDirection *= -1.f;
		}
		const float MaxSpeed = GetMovementComponent()->GetMaxSpeed();
		// --- Tangential movement ---
		const FVector OrbitVelocity = CalculateSmoothOrbitVelocity(RadialDirection, DeltaTime);
		// --- Radial movement ---
		const float RadiusError = PreferredDistance - Distance;
		FVector RadiusVelocity;
		if (RadiusError < -RadialDeadZone)
		{
			const float NormalizedError = FMath::Clamp(RadiusError / AnticipationDistance, 0.f, 1.f);
			const float ForwardSpeed = MaxSpeed * NormalizedError;
			RadiusVelocity = RadialDirection * ForwardSpeed;
		}
		else if (RadiusError > RadialDeadZone)
		{
			// Too close → back up slowly
			RadiusVelocity = RadialDirection * OrbitSpeed * 0.5f;
		}
		else
		{
			RadiusVelocity = FVector::ZeroVector;
		}
		const FVector DesiredVelocity = OrbitVelocity + RadiusVelocity;
		// --- Convert to input space ---
		const FVector Input = DesiredVelocity / MaxSpeed;
		return Input;
	}
	return FVector::ZeroVector;
}

bool UOrbitEngagementMovementPlugin::IsDesiredLocationNavigable(const FVector& DesiredLocation) const
{
	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation ProjectedLocation;
		// Keep Extent small for performance; it's the search radius
		const FVector Extent(50.f, 50.f, 250.f);

		return NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLocation, Extent);
	}
	return false;
}
