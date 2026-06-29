// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Actor/MagicHand/MagicHandTypes.h"
#include "TetherUpdateHandler.generated.h"

// Abstract UObject base for all tether handlers. Subclass this to define how a
// specific possessable object responds to being tethered to the player.
//
// Two methods are called each frame:
//
//   QueryAllowedMovement  — called BEFORE the CMC physics step (from
//                           UMagicHandCharacterMovementComponent::ConsumeInputVector).
//                           Returns the portion of the desired input the player is
//                           actually allowed to apply given the tether state.
//
//   UpdateTetherPhysics   — called AFTER the player has moved (from
//                           UMagicHandComponent::TickComponent). Moves/rotates the
//                           target to respond to the player's actual displacement.
UCLASS(Abstract, Blueprintable, BlueprintType)
class UTetherUpdateHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Tether")
	void Initialize();
	// Pre-movement constraint query. Override to restrict player movement when the
	// target cannot accommodate it (e.g. blocked by collision, at a spline endpoint).
	// The base implementation imposes no constraint and returns DesiredInputVector unchanged.
	UFUNCTION(BlueprintNativeEvent, Category="Tether")
	FVector QueryAllowedMovement(const FTetherHandlerContext& Context, const FVector& DesiredInputVector);
	virtual FVector QueryAllowedMovement_Implementation(const FTetherHandlerContext& Context, const FVector& DesiredInputVector);

	// Post-movement physics update. Move, rotate, or otherwise respond to the player
	// having moved by Context.BaseOwnerDelta. Return bLimitExceeded = true to force
	// the tether to break.
	UFUNCTION(BlueprintNativeEvent, Category="Tether")
	FTetherHandlerResult UpdateTetherPhysics(const FTetherHandlerContext& Context);
	virtual FTetherHandlerResult UpdateTetherPhysics_Implementation(const FTetherHandlerContext& Context);
};
