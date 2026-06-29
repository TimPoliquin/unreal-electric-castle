// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MagicHandTypes.generated.h"

class AActor;
class UMagicHandComponent;

UENUM(BlueprintType)
enum class EMagicHandState : uint8
{
	Idle,
	Casting,
	Tethered,
	Releasing
};

// Passed to every handler call each tick. Built by UMagicHandComponent and cached
// so that UMagicHandCharacterMovementComponent can query it before the CMC physics step.
USTRUCT(BlueprintType)
struct FTetherHandlerContext
{
	GENERATED_BODY()

	// The player character that owns the tether.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> OwnerActor = nullptr;

	// The object currently held by the tether.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float MaxTetherLength = 1000.f;

	UPROPERTY(BlueprintReadOnly)
	float TetherLengthPadding = 50.f;

	// Owner location at the END of the previous frame (before last CMC tick).
	UPROPERTY(BlueprintReadOnly)
	FVector PreviousOwnerLocation = FVector::ZeroVector;

	// Delta between PreviousOwnerLocation and the owner's position when this context was built.
	// One-frame lagged — mirrors the pattern in the reference TrackUpdateTetherHandler.
	UPROPERTY(BlueprintReadOnly)
	FVector BaseOwnerDelta = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FTetherHandlerResult
{
	GENERATED_BODY()

	// World-space distance between owner and target after the update.
	UPROPERTY(BlueprintReadOnly)
	float CalculatedDistance = 0.f;

	// True when the target is so far out of range that the tether should be broken.
	UPROPERTY(BlueprintReadOnly)
	bool bLimitExceeded = false;
};

USTRUCT(BlueprintType)
struct FMagicHandPossessEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> PossessorActor = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> PossessedActor = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMagicHandComponent> MagicHandComponent = nullptr;
};