// Copyright Alien Shores


#include "Utils/VectorUtils.h"

EVectorDirection UVectorUtils::GetInputVectorDirection(AActor* Actor, const FVector& InputDirection)
{
	if (FMath::IsNearlyZero(InputDirection.Length()))
	{
		return EVectorDirection::None;
	}
	const FVector Forward = Actor->GetActorForwardVector();
	const FVector Right = Actor->GetActorRightVector();
	FVector LocalInput = InputDirection;
	LocalInput.Z = 0.f;
	LocalInput.Normalize();
	const float ForwardDot = FVector::DotProduct(LocalInput, Forward);
	const float RightDot = FVector::DotProduct(LocalInput, Right);
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? EVectorDirection::Forward : EVectorDirection::Backward;
	}
	return RightDot >= 0.f ? EVectorDirection::Right : EVectorDirection::Left;
}

FVector UVectorUtils::ComputeOrbitTarget(const FVector& StartingLocation, const FVector& Center, const FVector& DodgeDirection, const float Distance)
{
	const FVector UpVector = FVector::UpVector;

	// Vector from target to player
	FVector R = StartingLocation - Center;
	R.Z = 0.f;

	if (R.IsNearlyZero())
	{
		// Degenerate case: fallback to character right vector
		return StartingLocation + (DodgeDirection) * Distance;
	}

	R.Normalize();

	// Tangent direction: perpendicular to R
	FVector TangentDir;
	if (DodgeDirection == FVector::RightVector)
	{
		TangentDir = FVector::CrossProduct(R, UpVector); // right tangent
	}
	else
	{
		TangentDir = FVector::CrossProduct(UpVector, R); // left tangent
	}

	TangentDir.Normalize();

	// Final warp target
	return StartingLocation + TangentDir * Distance;
}
