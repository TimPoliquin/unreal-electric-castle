// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/MagicHand/Handler/TetherUpdateHandler.h"
#include "TrackTetherHandler.generated.h"

class USplineComponent;

// Tether handler for actors that follow a spline track.
//
// QueryAllowedMovement (pre-tick constraint):
//   On open (non-looping) splines, removes the spline-aligned component of the
//   player's input when the target has reached a spline endpoint. The player
//   remains free to move in any direction perpendicular to the spline.
//   Closed-loop splines have no endpoints, so no constraint is applied.
//
// UpdateTetherPhysics (post-tick position update):
//   Uses a Newton-Raphson solver to find the point along the spline that sits at
//   the same distance from the player's new position as the target did from the
//   player's previous position, preserving tether length across movement. A
//   separate boundary-engagement pass handles the case where the player has moved
//   outside the tether radius and needs the target dragged to the nearest in-range
//   spline point.
//
// SplineComponent must be assigned before possession begins — typically set by
// the possessable actor inside GetTetherUpdateHandler() before returning this handler.
UCLASS(Blueprintable, BlueprintType)
class UTrackTetherHandler : public UTetherUpdateHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Track Tether", meta=(ExposeOnSpawn))
	TObjectPtr<USplineComponent> SplineComponent;

	// Distance from a spline endpoint (in world units) within which the boundary
	// movement constraint activates. Increase this if the player can "slip past"
	// the endpoint before the constraint kicks in at high frame-deltas.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track Tether", meta=(ExposeOnSpawn))
	float EndpointConstraintMargin = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track Tether", meta=(ExposeOnSpawn))
	bool bDebug = false;

	virtual FVector QueryAllowedMovement_Implementation(
		const FTetherHandlerContext& Context, const FVector& DesiredInputVector
	) override;

	virtual FTetherHandlerResult UpdateTetherPhysics_Implementation(
		const FTetherHandlerContext& Context
	) override;
};
