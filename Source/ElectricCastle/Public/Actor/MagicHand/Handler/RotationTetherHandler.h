// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/MagicHand/Handler/TetherUpdateHandler.h"
#include "RotationTetherHandler.generated.h"

USTRUCT(BlueprintType)
struct FRotationTetherHandlerEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	float Angle = 0.f;
};

UENUM(BlueprintType)
enum class ERotationTetherMode : uint8
{
	// Target rotates continuously. Each frame the torque derived from the
	// player's movement is applied directly as a world-space rotation delta.
	Free,

	// Rotation accumulates silently. Once the accumulated torque crosses
	// SnapThreshold the target snaps to the next SnapAngleDegrees increment.
	Snap
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotationTetherHandlerEventSignature, const FRotationTetherHandlerEventPayload&, Payload);

// Handler that treats the tether as a lever arm: the player's world-space
// movement is the "force" and the tether vector (target→player) is the "arm".
//
//   TorqueVector = TetherVector × PlayerMovementDelta
//
// The rotation axis is Normalize(TorqueVector) and the angular displacement is:
//
//   DeltaAngle = |PlayerDelta| * sin(theta) / |TetherVector| * RotationSensitivity
//
// This is the arc-length formula: perpendicular movement at radius r creates
// rotation angle = movement / r. Movement parallel to the tether generates no torque.
//
// Free mode applies the rotation immediately each frame.
//
// Snap mode accumulates rotation projected onto SnapPrimaryAxis and fires a
// discrete SnapAngleDegrees step each time SnapThreshold is crossed. Reversing
// direction subtracts from the accumulator, letting the player "undo" a snap.
//
// The target never translates; QueryAllowedMovement enforces the tether length
// cap with a simple distance projection (blocking outward-extending input).
UCLASS(Blueprintable, BlueprintType)
class URotationTetherHandler : public UTetherUpdateHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation Tether", meta=(ExposeOnSpawn))
	ERotationTetherMode Mode = ERotationTetherMode::Free;

	// Multiplier on the computed delta angle. Values above 1 feel "heavier";
	// values below 1 require more player movement per degree of rotation.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Rotation Tether",
		meta=(ClampMin="0.01", ExposeOnSpawn)
	)
	float RotationSensitivity = 1.f;

	// [Snap] Degrees applied per snap step.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Rotation Tether",
		meta=(EditCondition="Mode == ERotationTetherMode::Snap", EditConditionHides,
			ClampMin="1.0", ExposeOnSpawn)
	)
	float SnapAngleDegrees = 45.f;

	// [Snap] Accumulated torque (degrees) required to trigger one snap step.
	// Lower values = more responsive; higher values = harder to move.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Rotation Tether",
		meta=(EditCondition="Mode == ERotationTetherMode::Snap", EditConditionHides,
			ClampMin="1.0", ExposeOnSpawn)
	)
	float SnapThreshold = 80.f;

	// [Snap] World-space axis around which accumulation and snap steps are
	// measured. Defaults to Z (up), matching turntables and hinged doors.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Rotation Tether",
		meta=(EditCondition="Mode == ERotationTetherMode::Snap", EditConditionHides, ExposeOnSpawn)
	)
	FVector SnapPrimaryAxis = FVector::UpVector;
	
	UPROPERTY(BlueprintAssignable)
	FRotationTetherHandlerEventSignature OnRotationSnap;

	virtual FVector QueryAllowedMovement_Implementation(
		const FTetherHandlerContext& Context, const FVector& DesiredInputVector
	) override;

	virtual FTetherHandlerResult UpdateTetherPhysics_Implementation(
		const FTetherHandlerContext& Context
	) override;

private:
	// Signed degree accumulator for Snap mode.
	// Positive = counter-clockwise relative to SnapPrimaryAxis (right-hand rule).
	float AccumulatedSnapDegrees = 0.f;

	void ApplyFreeRotation(
		AActor* Target, float DeltaAngleDegrees,
		const FVector& RotationAxis
	) const;

	void AccumulateAndSnapRotation(
		AActor* Target, float DeltaAngleDegrees,
		const FVector& TorqueAxis
	);
};
