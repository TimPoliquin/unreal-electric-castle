// Copyright Alien Shores


#include "AI/Engagement/Movement/OrbitEngagementMovementPlugin.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AI/Targeting/AITargetingComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Utils/RandUtils.h"

void UOrbitEngagementMovementPlugin::InitializeDependencies_Implementation(AActor* InOwnerActor)
{
	Super::InitializeDependencies_Implementation(InOwnerActor);
	bOrbitClockwise = URandRangeBlueprintLibrary::ShouldAct(.5f);
	OrbitSpeed = URandRangeBlueprintLibrary::GetRandomFloatInRange(OrbitSpeedRange);
}

void UOrbitEngagementMovementPlugin::TickMovement_Implementation(const float DeltaTime)
{
	if (!GetMovementComponent())
	{
		return;
	}
	if (!IsValid(GetTargetActor()))
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
		if (Distance - RadialDeadZone <= 0.0f) // Prevent division by zero if exactly on top of target
		{
			return FVector::ZeroVector;
		}

		const FVector RadialDirection = DirectionToTarget / Distance; // Points from Target to Owner
		const float MaxSpeed = GetMovementComponent()->GetMaxSpeed();

		// 1. Calculate Base Tangential (Orbit) Velocity
		FVector OrbitVelocity = CalculateSmoothOrbitVelocity(RadialDirection, DeltaTime);

		// 2. Calculate Radial Velocity & Priorities
		const float RadiusError = PreferredDistance - Distance;
		const float AbsoluteRadiusError = FMath::Abs(RadiusError);

		FVector RadiusVelocity = FVector::ZeroVector;

		if (AbsoluteRadiusError > RadialDeadZone)
		{
			// Calculate how far out of bounds we are, relative to our anticipation window
			const float ExcessError = AbsoluteRadiusError - RadialDeadZone;
			const float RadialWeight = FMath::Clamp(ExcessError / AnticipationDistance, 0.f, 1.f);

			if (RadiusError < 0.f)
			{
				// TOO FAR: Move towards target (-RadialDirection) at a speed proportional to the error
				const float ForwardSpeed = MaxSpeed * RadialWeight;
				RadiusVelocity = -RadialDirection * ForwardSpeed;
			}
			else
			{
				// TOO CLOSE: Back up along RadialDirection
				RadiusVelocity = RadialDirection * (OrbitSpeed * 0.5f);
			}

			// Prioritize radial over tangential: Suppress orbiting while closing/opening the gap
			const float TangentialWeight = 1.f - RadialWeight;
			OrbitVelocity *= TangentialWeight;
		}

		// 3. Combine and normalize to input space
		const FVector DesiredVelocity = OrbitVelocity + RadiusVelocity;
		const FVector Input = DesiredVelocity / MaxSpeed;

		if (bDebug)
		{
			DrawDebugDirectionalArrow(GetWorld(), OwnerActor->GetActorLocation(), OwnerActor->GetActorLocation() + Input * 100.f, 4.f, FColor::Red, false, 0.f, 0, 1.5f);
		}

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
