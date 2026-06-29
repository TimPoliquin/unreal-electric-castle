// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/MagicHand/Handler/TetherUpdateHandler.h"
#include "PhysicsMovementTetherHandler.generated.h"

// Handler that makes the tethered actor mirror the player's world-space movement,
// sweeping for collisions along the path.
//
// QueryAllowedMovement:
//   When the tether is near its limit and the player's input would extend it
//   further, this method sweeps the target in the input direction. If the target
//   is blocked by world geometry the tether-extending component is removed from
//   the player's input — the player can still move laterally. If the target can
//   partially follow, the extending component is scaled by the sweep's Time value.
//
// UpdateTetherPhysics:
//   Moves the target by Context.BaseOwnerDelta (the actual player displacement
//   from last frame), clamped to remain within the tether sphere, then sweeps for
//   collisions and stops at the first blocking hit.
UCLASS(Blueprintable, BlueprintType)
class UPhysicsMovementTetherHandler : public UTetherUpdateHandler
{
	GENERATED_BODY()

public:
	// Radius of the sphere used for all sweep tests.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Tether", meta=(ExposeOnSpawn))
	float SweepRadius = 30.f;

	// How far ahead (in the input direction) the constraint sweep tests.
	// Larger values give more look-ahead but may falsely block movement near corners.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Tether", meta=(ExposeOnSpawn))
	float ConstraintTestDistance = 150.f;

	// Distance below the tether limit at which constraint tests begin.
	// Prevents unnecessary sweeps when the player is comfortably inside range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Tether", meta=(ExposeOnSpawn))
	float ConstraintActivationMargin = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Tether", meta=(ExposeOnSpawn))
	bool bDebug = false;

	virtual void Initialize_Implementation() override;

	virtual FVector QueryAllowedMovement_Implementation(
		const FTetherHandlerContext& Context, const FVector& DesiredInputVector
	) override;

	virtual FTetherHandlerResult UpdateTetherPhysics_Implementation(
		const FTetherHandlerContext& Context
	) override;

protected:
	FCollisionShape CreateCollisionShape() const;

private:
	// Returns true if a sphere sweep from StartLoc to EndLoc hits ECC_WorldStatic geometry.
	// OutHit.Time carries the fraction of the sweep completed before blocking (0 = immediately blocked).
	bool SweepTestForMovement(
		const AActor* Actor,
		const FVector& StartLoc, const FVector& EndLoc, FHitResult& OutHit
	) const;
	bool ResolveBlockedMovement(
		const AActor* TargetActor, const FVector& CurrentTargetLoc, const FVector& DesiredInputVector, const FVector& TetherDir, const FHitResult& SweepHit, FVector& OutAdjustedInput
	) const;
	FCollisionShape CollisionShape;
};
