// Copyright Alien Shores

#include "Actor/MagicHand/Handler/PhysicsMovementTetherHandler.h"
#include "DrawDebugHelpers.h" // Add this header at the top of the file


// HELPER FUNCTION: Resolves whether a target's collision sweep is an absolute block 
// or if it's a side-wall slide / clean departure from an initial flush contact.
bool UPhysicsMovementTetherHandler::ResolveBlockedMovement(
	const AActor* TargetActor,
	const FVector& CurrentTargetLoc,
	const FVector& DesiredInputVector,
	const FVector& TetherDir,
	const FHitResult& SweepHit,
	FVector& OutAdjustedInput
) const
{
	OutAdjustedInput = DesiredInputVector;

	if (SweepHit.Time < KINDA_SMALL_NUMBER || SweepHit.bStartPenetrating)
	{
		// Derive a reliable surface normal pointing away from the obstacle by checking
		// the vector from the impact point back to our centered sweep start position.
		const FVector VerticalOffset = FVector::UpVector * (SweepRadius / 2.f);
		FVector EffectiveNormal = SweepHit.Normal;

		if (SweepHit.bStartPenetrating && !SweepHit.ImpactPoint.IsZero())
		{
			EffectiveNormal = ((CurrentTargetLoc + VerticalOffset) - SweepHit.ImpactPoint).GetSafeNormal();
			EffectiveNormal.Z = 0.f;
			EffectiveNormal.Normalize();
		}

		// 1. FLOOR CHECK: Ground should not arrest horizontal momentum
		if (SweepHit.Normal.Z > 0.7f && !SweepHit.bStartPenetrating)
		{
			return false; // Not a true structural block
		}

		// 2. CORRIDOR ORTHOGONALITY CHECK:
		const FVector MoveDir = DesiredInputVector.GetSafeNormal();
		const float WallAlignment = FMath::Abs(FVector::DotProduct(MoveDir, EffectiveNormal));
		if (WallAlignment < 0.15f)
		{
			return false; // Side-wall scrape, allow unhindered movement
		}

		// 3. TRUE ESCAPE CHECK: Moving away from the wall
		if (FVector::DotProduct(DesiredInputVector, EffectiveNormal) > 0.f)
		{
			return false; // Backing out, allow unhindered movement
		}

		// 4. HARD FRONT-FACING BLOCK: Sliding along the tether plane
		const FVector SlidInput = FVector::VectorPlaneProject(DesiredInputVector, TetherDir);
		if (FVector::DotProduct(SlidInput, DesiredInputVector) < 0.f)
		{
			OutAdjustedInput = FVector::ZeroVector;
		}
		else
		{
			OutAdjustedInput = SlidInput;
		}
		return true; // True block applied
	}

	return true;
}

void UPhysicsMovementTetherHandler::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	CollisionShape = CreateCollisionShape();
}

FVector UPhysicsMovementTetherHandler::QueryAllowedMovement_Implementation(
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

	const float ActivationThresholdSq = FMath::Square(AllowedMax - ConstraintActivationMargin);
	if (FVector::DistSquared(CurrentOwnerLoc, CurrentTargetLoc) < ActivationThresholdSq)
	{
		return DesiredInputVector;
	}

	const FVector TetherDir = (CurrentOwnerLoc - CurrentTargetLoc).GetSafeNormal();
	const float ExtensionDot = FVector::DotProduct(DesiredInputVector, TetherDir);

	if (ExtensionDot <= 0.f)
	{
		return DesiredInputVector;
	}

	const FVector SweepEnd = CurrentTargetLoc + DesiredInputVector.GetSafeNormal() * ConstraintTestDistance;
	const FVector VerticalOffset = FVector::ZeroVector;

	FHitResult SweepHit;
	const bool bTargetBlocked = SweepTestForMovement(
		Context.TargetActor,
		CurrentTargetLoc + VerticalOffset,
		SweepEnd + VerticalOffset,
		SweepHit
	);

	if (!bTargetBlocked)
	{
		return DesiredInputVector;
	}

	FVector AdjustedInput;
	if (ResolveBlockedMovement(Context.TargetActor, CurrentTargetLoc, DesiredInputVector, TetherDir, SweepHit, AdjustedInput))
	{
		// If it's a true, inescapable block, scale down the extending component natively
		if (AdjustedInput.Equals(DesiredInputVector))
		{
			const FVector TangentialInput = DesiredInputVector - TetherDir * ExtensionDot;
			const FVector ExtendingInput = TetherDir * ExtensionDot;
			return TangentialInput + ExtendingInput * SweepHit.Time;
		}
		return AdjustedInput;
	}

	return DesiredInputVector;
}

FTetherHandlerResult UPhysicsMovementTetherHandler::UpdateTetherPhysics_Implementation(
	const FTetherHandlerContext& Context
)
{
	FTetherHandlerResult Result;

	if (!Context.OwnerActor || !Context.TargetActor)
	{
		return Result;
	}

	const FVector CurrentOwnerLoc = Context.OwnerActor->GetActorLocation();
	const FVector CurrentTargetLoc = Context.TargetActor->GetActorLocation();
	const float AllowedMax = Context.MaxTetherLength + Context.TetherLengthPadding;

	// Calculate baseline movement mirroring the player
	const FVector BaseDelta = Context.BaseOwnerDelta;
	FVector FinalDelta = BaseDelta;

	const FVector VerticalOffset = FVector::ZeroVector;

	FHitResult SweepHit;
	const bool bHit = SweepTestForMovement(
		Context.TargetActor,
		CurrentTargetLoc + VerticalOffset,
		(CurrentTargetLoc + BaseDelta) + VerticalOffset,
		SweepHit
	);

	if (bHit)
	{
		const FVector TetherDir = (CurrentOwnerLoc - CurrentTargetLoc).GetSafeNormal();
		FVector AdjustedDelta;

		bool bIsTrueBlock = ResolveBlockedMovement(
			Context.TargetActor,
			CurrentTargetLoc,
			BaseDelta,
			TetherDir,
			SweepHit,
			AdjustedDelta
		);

		if (bIsTrueBlock)
		{
			// SMOOTH SLIDE FIX: Instead of hard-teleporting to SweepHit.Location or manually 
			// calculating penetration offsets, project our movement delta onto the wall surface.
			// This strips the vector component pushing into the wall, ensuring the box perfectly 
			// slides along it without ever penetrating.

			FVector WallNormal = SweepHit.Normal;

			// Handle penetration corruption safely
			if (SweepHit.bStartPenetrating && !SweepHit.ImpactPoint.IsZero())
			{
				WallNormal = ((CurrentTargetLoc + VerticalOffset) - SweepHit.ImpactPoint).GetSafeNormal();
				WallNormal.Z = 0.f;
				WallNormal.Normalize();
			}

			// Slide the baseline delta perfectly flush along the wall normal
			FinalDelta = FVector::VectorPlaneProject(BaseDelta, WallNormal);

			// Ensure sliding doesn't reverse our original movement direction due to precision errors
			if (FVector::DotProduct(FinalDelta, BaseDelta) < 0.f)
			{
				FinalDelta = FVector::ZeroVector;
			}
		}
		else
		{
			// Box is slipping out of a corridor or backing away, allow unhindered delta movement
			FinalDelta = AdjustedDelta;
		}
	}

	// Calculate final target coordinates
	// Calculate final target coordinates
	FVector FinalTargetLoc = CurrentTargetLoc + FinalDelta;

	// FIX: Instead of locking the Z axis to the target's current height,
	// only maintain vertical matching if the player's delta actually includes vertical movement (like a jump!).
	if (FMath::IsNearlyZero(BaseDelta.Z))
	{
		// If the player isn't changing elevation (jumping/falling), keep the box's current Z height
		// so it doesn't drift due to floating-point precision errors during horizontal slides.
		FinalTargetLoc.Z = CurrentTargetLoc.Z;
	}
	else
	{
		// The player is jumping or falling! Allow the box's final Z to match the calculated trajectory.
		FinalTargetLoc.Z = CurrentTargetLoc.Z + FinalDelta.Z;
	}

	if (!FinalTargetLoc.Equals(CurrentTargetLoc, 0.1f))
	{
		FHitResult MoveHit;
		Context.TargetActor->SetActorLocation(
			FinalTargetLoc,
			true,
			// Sweeping keeps it from going through ceilings/floors when following a jump
			&MoveHit,
			ETeleportType::TeleportPhysics
		);
	}

	Result.CalculatedDistance = FVector::Distance(CurrentOwnerLoc, Context.TargetActor->GetActorLocation());

	if (Result.CalculatedDistance > AllowedMax + KINDA_SMALL_NUMBER)
	{
		if (Result.CalculatedDistance > AllowedMax * 1.5f)
		{
			Result.bLimitExceeded = true;
		}
	}

	return Result;
}

FCollisionShape UPhysicsMovementTetherHandler::CreateCollisionShape() const
{
	return FCollisionShape::MakeSphere(SweepRadius);
}


bool UPhysicsMovementTetherHandler::SweepTestForMovement(const AActor* Actor, const FVector& StartLoc, const FVector& EndLoc, FHitResult& OutHit) const
{
	if (!Actor || !Actor->GetWorld() || StartLoc.Equals(EndLoc, 0.1f))
	{
		return false;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);

	// Fire the actual sweep channel query
	const bool bHit = Actor->GetWorld()->SweepSingleByChannel(
		OutHit,
		StartLoc,
		EndLoc,
		Actor->GetActorQuat(),
		ECC_WorldStatic,
		CollisionShape,
		Params
	);
	return bHit;
}
