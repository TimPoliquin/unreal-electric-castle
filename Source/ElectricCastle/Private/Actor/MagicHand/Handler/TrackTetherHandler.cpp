// Copyright Alien Shores

#include "Actor/MagicHand/Handler/TrackTetherHandler.h"
#include "Actor/MagicHand/MagicHandComponent.h"
#include "Components/SplineComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

FVector UTrackTetherHandler::QueryAllowedMovement_Implementation(
	const FTetherHandlerContext& Context, const FVector& DesiredInputVector
)
{
	if (!Context.OwnerActor || !Context.TargetActor || !SplineComponent || DesiredInputVector.IsNearlyZero())
	{
		return DesiredInputVector;
	}

	if (SplineComponent->IsClosedLoop())
	{
		return DesiredInputVector;
	}

	const FVector OwnerLoc = Context.OwnerActor->GetActorLocation();
	const FVector CurrentTargetLoc = Context.TargetActor->GetActorLocation();
	const float SplineLength = SplineComponent->GetSplineLength();

	const float CurrentDistance = SplineComponent->GetDistanceAlongSplineAtLocation(
		CurrentTargetLoc,
		ESplineCoordinateSpace::World
	);

	const bool bAtStart = CurrentDistance <= EndpointConstraintMargin;
	const bool bAtEnd = CurrentDistance >= SplineLength - EndpointConstraintMargin;

	if (!bAtStart && !bAtEnd)
	{
		return DesiredInputVector;
	}

	FVector TrackingAnchorLoc = CurrentTargetLoc;
	if (const UMagicHandComponent* MagicHandComp = Context.OwnerActor->FindComponentByClass<UMagicHandComponent>())
	{
		TrackingAnchorLoc = MagicHandComp->GetAnchorLocation();
	}

	const float AllowedMax = Context.MaxTetherLength + Context.TetherLengthPadding;
	const float CurrentLeashDistance = FVector::Distance(OwnerLoc, TrackingAnchorLoc);

	// RIGID LEASH CONSTRAINT: Restrict player movement exclusively when hit at maximum leash radius
	if (CurrentLeashDistance >= AllowedMax - 1.f)
	{
		FVector AwayFromAnchorDir = (OwnerLoc - TrackingAnchorLoc).GetSafeNormal();
		float PullingForce = FVector::DotProduct(DesiredInputVector, AwayFromAnchorDir);

		if (PullingForce > 0.f)
		{
			return DesiredInputVector - (AwayFromAnchorDir * PullingForce);
		}
	}

	return DesiredInputVector;
}

FTetherHandlerResult UTrackTetherHandler::UpdateTetherPhysics_Implementation(
	const FTetherHandlerContext& Context
)
{
	FTetherHandlerResult Result;

	if (!Context.OwnerActor || !Context.TargetActor || !SplineComponent)
	{
		return Result;
	}

	const FVector OwnerLoc = Context.OwnerActor->GetActorLocation();
	const FVector CurrentLoc = Context.TargetActor->GetActorLocation();
	const float SplineLength = SplineComponent->GetSplineLength();
	const float AllowedMax = Context.MaxTetherLength + Context.TetherLengthPadding;

	const float CurrentDistanceAlongSpline = SplineComponent->GetDistanceAlongSplineAtLocation(
		CurrentLoc,
		ESplineCoordinateSpace::World
	);

	FVector LocalAnchorOffset = FVector::ZeroVector;
	FVector WorldAnchorLoc = CurrentLoc;
	if (const UMagicHandComponent* MagicHandComp = Context.OwnerActor->FindComponentByClass<UMagicHandComponent>())
	{
		WorldAnchorLoc = MagicHandComp->GetAnchorLocation();
		LocalAnchorOffset = Context.TargetActor->GetActorQuat().UnrotateVector(WorldAnchorLoc - CurrentLoc);
	}

	// 1. Unified Unified Rigid Rod Projection Pass
	const FVector SplineTangent = SplineComponent->GetTangentAtDistanceAlongSpline(
		CurrentDistanceAlongSpline,
		ESplineCoordinateSpace::World
	).GetSafeNormal();

	FVector DirToPlayer = (OwnerLoc - WorldAnchorLoc).GetSafeNormal();
	float OutwardPlayerDelta = FVector::DotProduct(Context.BaseOwnerDelta, DirToPlayer);

	float AppliedMovementStep = 0.f;

	// Determine if the frame movement is an outward pull or an aligned push vector
	if (OutwardPlayerDelta > 0.01f)
	{
		float TrackDirectionSign = FVector::DotProduct(SplineTangent, DirToPlayer);
		AppliedMovementStep = (TrackDirectionSign > 0.f) ? OutwardPlayerDelta : -OutwardPlayerDelta;
	}
	else
	{
		AppliedMovementStep = FVector::DotProduct(Context.BaseOwnerDelta, SplineTangent);
	}

	float EstimatedDistance = FMath::Clamp(CurrentDistanceAlongSpline + AppliedMovementStep, 0.f, SplineLength);

	if (bDebug)
	{
		// --- LOG CONSOLE METRICS FOR VERIFICATION ---
		UE_LOG(LogElectricCastle, Log, TEXT("==== [TrackTether Optimized Rigid Diagnostics] ===="));
		UE_LOG(LogElectricCastle, Log, TEXT(" - Player Location:     %s | Base Owner Delta: %s"), *OwnerLoc.ToString(), *Context.BaseOwnerDelta.ToString());
		UE_LOG(LogElectricCastle, Log, TEXT(" - Box Center Location: %s | Hand Anchor World Location: %s"), *CurrentLoc.ToString(), *WorldAnchorLoc.ToString());
		UE_LOG(LogElectricCastle, Log, TEXT(" - Applied Movement Step along Spline: %.4f"), AppliedMovementStep);
		UE_LOG(LogElectricCastle, Log, TEXT(" - Target Spline Distance resolved to: %.2f (Was: %.2f)"), EstimatedDistance, CurrentDistanceAlongSpline);
	}

	// 2. Bound Calibration Search Pass (Only executes when constraints are broken)
	FVector ProposedCenterLoc = SplineComponent->GetLocationAtDistanceAlongSpline(EstimatedDistance, ESplineCoordinateSpace::World);
	FQuat ProposedRot = SplineComponent->GetQuaternionAtDistanceAlongSpline(EstimatedDistance, ESplineCoordinateSpace::World);
	FVector ProposedAnchorLoc = ProposedCenterLoc + ProposedRot.RotateVector(LocalAnchorOffset);

	if (float ProposedDistance = FVector::Distance(OwnerLoc, ProposedAnchorLoc); ProposedDistance > AllowedMax)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Log, TEXT(" -> Distance limits breached (%.2f > %.2f). Executing safety calibration search..."), ProposedDistance, AllowedMax);
		}

		float Low = (EstimatedDistance > CurrentDistanceAlongSpline) ? CurrentDistanceAlongSpline : 0.f;
		float High = (EstimatedDistance > CurrentDistanceAlongSpline) ? EstimatedDistance : CurrentDistanceAlongSpline;

		constexpr int32 SearchSteps = 12;
		float BestDistance = CurrentDistanceAlongSpline;

		for (int32 Step = 0; Step < SearchSteps; ++Step)
		{
			float Mid = (Low + High) * 0.5f;
			FVector MidCenter = SplineComponent->GetLocationAtDistanceAlongSpline(Mid, ESplineCoordinateSpace::World);
			FQuat MidRot = SplineComponent->GetQuaternionAtDistanceAlongSpline(Mid, ESplineCoordinateSpace::World);
			FVector MidAnchor = MidCenter + MidRot.RotateVector(LocalAnchorOffset);

			if (FVector::Distance(OwnerLoc, MidAnchor) <= AllowedMax)
			{
				BestDistance = Mid;
				Low = Mid; // Nudge boundaries closer to target spacing thresholds
			}
			else
			{
				High = Mid;
			}
		}

		EstimatedDistance = FMath::Clamp(BestDistance, 0.f, SplineLength);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Log, TEXT(" -> Bounded search converged on stable Spline Distance: %.2f"), EstimatedDistance);
		}

		// Recalculate positions based on the calibrated distance fallback
		ProposedCenterLoc = SplineComponent->GetLocationAtDistanceAlongSpline(EstimatedDistance, ESplineCoordinateSpace::World);
		ProposedRot = SplineComponent->GetQuaternionAtDistanceAlongSpline(EstimatedDistance, ESplineCoordinateSpace::World);
		ProposedAnchorLoc = ProposedCenterLoc + ProposedRot.RotateVector(LocalAnchorOffset);
	}

	Result.CalculatedDistance = FVector::Distance(OwnerLoc, ProposedAnchorLoc);

	// 3. Absolute Safety Fallback Guard
	if (Result.CalculatedDistance > AllowedMax + 5.f)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT(" !!! CRITICAL !!! Position out of bounds after safety loops (%.1f > %.1f). Dropping frame update."), Result.CalculatedDistance, AllowedMax);
		}

		EstimatedDistance = CurrentDistanceAlongSpline;
		ProposedCenterLoc = CurrentLoc;

		FQuat ResetRot = SplineComponent->GetQuaternionAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		Result.CalculatedDistance = FVector::Distance(OwnerLoc, CurrentLoc + ResetRot.RotateVector(LocalAnchorOffset));

		if (Result.CalculatedDistance > AllowedMax * 1.5f)
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Error, TEXT(" !!! TETHER OVERSTRETCHED !!! Terminating active possession loop."));
			}
			Result.bLimitExceeded = true;
			return Result;
		}
	}

	// 4. Debug Render Visualizers
	if (bDebug && Context.OwnerActor->GetWorld())
	{
		DrawDebugSphere(Context.OwnerActor->GetWorld(), OwnerLoc, 25.f, 8, FColor::Blue, false, -1.f, 0, 2.f);
		DrawDebugSphere(Context.OwnerActor->GetWorld(), ProposedCenterLoc, 30.f, 8, FColor::Green, false, -1.f, 0, 2.f);
		DrawDebugSphere(Context.OwnerActor->GetWorld(), ProposedAnchorLoc, 20.f, 8, FColor::Red, false, -1.f, 0, 2.f);
		DrawDebugLine(Context.OwnerActor->GetWorld(), OwnerLoc, ProposedAnchorLoc, FColor::Orange, false, -1.f, 0, 3.f);
	}

	// 5. Commit Vector Modifications Natively
	if (!FMath::IsNearlyEqual(EstimatedDistance, CurrentDistanceAlongSpline, 0.01f))
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Log, TEXT(" -> Committing transformation update! Delta: %.4f"), (EstimatedDistance - CurrentDistanceAlongSpline));
		}
		Context.TargetActor->SetActorLocation(
			ProposedCenterLoc,
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);
	}

	return Result;
}
