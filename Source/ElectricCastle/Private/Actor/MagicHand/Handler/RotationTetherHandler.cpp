// Copyright Alien Shores

#include "Actor/MagicHand/Handler/RotationTetherHandler.h"
#include "GameFramework/Actor.h"

FVector URotationTetherHandler::QueryAllowedMovement_Implementation(
	const FTetherHandlerContext& Context, const FVector& DesiredInputVector
)
{
	if (!Context.OwnerActor || !Context.TargetActor || DesiredInputVector.IsNearlyZero())
	{
		return DesiredInputVector;
	}

	const FVector CurrentOwnerLoc = Context.OwnerActor->GetActorLocation();
	const FVector CurrentTargetLoc = Context.TargetActor->GetActorLocation();
	const float AllowedMax = Context.MaxTetherLength + Context.TetherLengthPadding;

	// The target never translates, so the tether grows as the player walks away.
	// When the player is inside the limit, all movement is permitted.
	if (FVector::DistSquared(CurrentOwnerLoc, CurrentTargetLoc) < FMath::Square(AllowedMax))
	{
		return DesiredInputVector;
	}

	// At the limit — remove any component of input that would stretch the tether further.
	const FVector TetherDir = (CurrentOwnerLoc - CurrentTargetLoc).GetSafeNormal();
	const float ExtensionAmount = FVector::DotProduct(DesiredInputVector, TetherDir);

	if (ExtensionAmount <= 0.f)
	{
		return DesiredInputVector;
	}

	return DesiredInputVector - TetherDir * ExtensionAmount;
}

FTetherHandlerResult URotationTetherHandler::UpdateTetherPhysics_Implementation(
	const FTetherHandlerContext& Context
)
{
	FTetherHandlerResult Result;

	if (!Context.OwnerActor || !Context.TargetActor || Context.BaseOwnerDelta.IsNearlyZero())
	{
		return Result;
	}

	const FVector CurrentOwnerLoc = Context.OwnerActor->GetActorLocation();
	const FVector CurrentTargetLoc = Context.TargetActor->GetActorLocation();

	// TetherVector is the lever arm: pivot at the target, force applied at the owner.
	const FVector TetherVector = CurrentOwnerLoc - CurrentTargetLoc;
	const float TetherLengthSq = TetherVector.SizeSquared();

	if (TetherLengthSq < SMALL_NUMBER)
	{
		return Result;
	}

	// Torque = r × F   (lever arm × applied force)
	const FVector TorqueVector = FVector::CrossProduct(TetherVector, Context.BaseOwnerDelta);
	const float TorqueMagnitude = TorqueVector.Size();

	if (TorqueMagnitude < KINDA_SMALL_NUMBER)
	{
		// Player moved parallel to the tether — no torque, no rotation.
		return Result;
	}

	const FVector RotationAxis = TorqueVector.GetSafeNormal();

	// Arc-length formula: angle = arc_length / radius
	//   TorqueMagnitude = |TetherVector| * |PlayerDelta| * sin(theta)
	//   DeltaAngle_rad  = TorqueMagnitude / |TetherVector|^2
	//                   = |PlayerDelta| * sin(theta) / |TetherVector|
	const float DeltaAngleRad = (TorqueMagnitude / TetherLengthSq) * RotationSensitivity;
	const float DeltaAngleDeg = FMath::RadiansToDegrees(DeltaAngleRad);

	if (Mode == ERotationTetherMode::Free)
	{
		ApplyFreeRotation(Context.TargetActor, DeltaAngleDeg, RotationAxis);
	}
	else
	{
		AccumulateAndSnapRotation(Context.TargetActor, DeltaAngleDeg, RotationAxis);
	}

	Result.CalculatedDistance = FMath::Sqrt(TetherLengthSq);
	return Result;
}

void URotationTetherHandler::ApplyFreeRotation(
	AActor* Target, float DeltaAngleDegrees,
	const FVector& RotationAxis
) const
{
	const FQuat DeltaRotation(RotationAxis, FMath::DegreesToRadians(DeltaAngleDegrees));
	Target->SetActorRotation(
		(DeltaRotation * Target->GetActorQuat()).Rotator(),
		ETeleportType::TeleportPhysics
	);
}

void URotationTetherHandler::AccumulateAndSnapRotation(
	AActor* Target, float DeltaAngleDegrees,
	const FVector& TorqueAxis
)
{
	// Project the torque direction onto the configured snap axis to determine
	// CW vs CCW. Positive dot → positive (CCW by right-hand rule) accumulation;
	// negative dot → clockwise. This means the player can reverse direction to
	// subtract from the accumulator and "undo" a pending snap step.
	const FVector NormalizedSnapAxis = SnapPrimaryAxis.GetSafeNormal();
	const float DirectionSign = FMath::Sign(FVector::DotProduct(TorqueAxis, NormalizedSnapAxis));

	AccumulatedSnapDegrees += DeltaAngleDegrees * DirectionSign;

	if (FMath::Abs(AccumulatedSnapDegrees) < SnapThreshold)
	{
		return;
	}

	// Threshold crossed — fire one discrete step and carry over the remainder.
	const float StepSign = FMath::Sign(AccumulatedSnapDegrees);
	const FQuat SnapRotation(
		NormalizedSnapAxis,
		FMath::DegreesToRadians(StepSign * SnapAngleDegrees)
	);

	Target->SetActorRotation(
		(SnapRotation * Target->GetActorQuat()).Rotator(),
		ETeleportType::TeleportPhysics
	);
	
	AccumulatedSnapDegrees -= StepSign * SnapThreshold;
	OnRotationSnap.Broadcast(FRotationTetherHandlerEventPayload(StepSign * SnapThreshold));
}
