// Copyright Alien Shores


#include "Player/LockOn/LockOnFunctionLibrary.h"

#include "ElectricCastle/ElectricCastle.h"
#include "Engine/OverlapResult.h"
#include "Interaction/CombatInterface.h"

AActor* ULockOnFunctionLibrary::FindClosestTarget(const AActor* TargetingActor, const float LockOnRadius, const float MaxLockOnDistance, const bool bRequireLineOfSight)
{
	if (!IsValid(TargetingActor))
	{
		return nullptr;
	}
	TArray<FCandidateEntry> Candidates = GatherCandidates(TargetingActor, LockOnRadius, MaxLockOnDistance, bRequireLineOfSight);
	if (Candidates.IsEmpty())
	{
		return nullptr;
	}

	AActor* Closest = nullptr;
	float ClosestDist = BIG_NUMBER;

	for (const FCandidateEntry& Candidate : Candidates)
	{
		if (Candidate.Distance < ClosestDist)
		{
			ClosestDist = Candidate.Distance;
			Closest = Candidate.Target;
		}
	}

	return Closest;
}

AActor* ULockOnFunctionLibrary::SwitchTarget(
	const AActor* TargetingActor,
	const AActor* CurrentTarget,
	const FVector2D& StickInput,
	const FRotator& CameraRotation,
	const float LockOnRadius,
	const float MaxLockOnDistance,
	const bool bRequireLineOfSight,
	const bool bDebug
)
{
	constexpr float MinSwitchDotThreshold = 0.34f;
	constexpr float DistanceWeight = .1f;
	// --- 1. Build world-space search direction from stick + camera axes ----
	//
	// We project the camera's Right and Forward vectors onto the world XY
	// plane so that vertical camera pitch does not distort horizontal flicks.
	// The resulting SearchDir is always a flat, horizontal world vector.

	const FVector CamFwd = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X);
	const FVector CamRight = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);

	// Project onto XY and re-normalise so vertical camera tilt is ignored.
	const FVector FwdXY = FVector(CamFwd.X, CamFwd.Y, 0.f).GetSafeNormal();
	const FVector RightXY = FVector(CamRight.X, CamRight.Y, 0.f).GetSafeNormal();

	// StickInput.X = right/left,  StickInput.Y = forward/back
	const FVector SearchDir = (RightXY * StickInput.X + FwdXY * StickInput.Y).GetSafeNormal();

	if (SearchDir.IsNearlyZero()) { return nullptr; }

	// --- 2. Gather valid candidates (excludes current target) --------------

	const FVector CurrentTargetLoc = IsValid(CurrentTarget)
		                                 ? CurrentTarget->GetActorLocation()
		                                 : TargetingActor->GetActorLocation();

	TArray<FCandidateEntry> Candidates = GatherCandidates(TargetingActor, LockOnRadius, MaxLockOnDistance, bRequireLineOfSight);
	Candidates.RemoveAll([&](const FCandidateEntry& A) { return A.Target == CurrentTarget; });

	if (Candidates.IsEmpty()) { return nullptr; }

	// --- 3. Score every candidate ------------------------------------------
	//
	// score = lerp(dot^2, 1/distance, DistanceWeight)
	//
	// "dot" is the cosine of the angle between the flat vector pointing from
	// the current target to the candidate, and the camera-relative stick dir.
	// Squaring it makes the score drop off sharply outside ±~45 degrees while
	// remaining smooth near zero degrees.

	AActor* BestCandidate = nullptr;
	float BestScore = -BIG_NUMBER;

	for (const FCandidateEntry& Candidate : Candidates)
	{
		if (Candidate.Distance < SMALL_NUMBER) { continue; }

		const FVector ToCandidate3D = Candidate.Target->GetActorLocation() - CurrentTargetLoc;
		// Flatten to XY for consistent angular scoring regardless of height.
		const FVector ToCandidateXY = FVector(ToCandidate3D.X, ToCandidate3D.Y, 0.f).GetSafeNormal();

		const float Dot = FVector::DotProduct(ToCandidateXY, SearchDir);

		// Reject candidates clearly outside the acceptance cone.
		if (Dot < MinSwitchDotThreshold) { continue; }

		// Angular component: dot^2 emphasises alignment without being binary.
		const float AngularScore = Dot * Dot;

		// Distance component: closer is better, normalised to [0,1] roughly.
		// Using MaxLockOnDistance as a soft normaliser so the two terms are
		// in a comparable range.
		const float InvDistScore = FMath::Clamp(1.f - (Candidate.Distance / MaxLockOnDistance), 0.f, 1.f);

		const float Score = FMath::Lerp(AngularScore, InvDistScore, DistanceWeight);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestCandidate = Candidate.Target;
		}

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			const FVector TextLoc = Candidate.Target->GetActorLocation() + FVector(0.f, 0.f, 60.f);
			const FString Text = FString::Printf(TEXT("Score: %.3f"), Score);

			DrawDebugString(
				TargetingActor->GetWorld(),
				TextLoc,
				Text,
				nullptr,
				FColor::Cyan,
				5.f,
				// Duration
				true // Draw shadow for readability
			);
		}
#endif
	}

#if ENABLE_DRAW_DEBUG
	// Optional: visualise the search direction in editor/dev builds.
	if (bDebug)
	{
		DrawDebugDirectionalArrow(
			TargetingActor->GetWorld(),
			CurrentTargetLoc,
			CurrentTargetLoc + SearchDir * 200.f,
			20.f,
			FColor::Yellow,
			false,
			0.5f,
			0,
			2.f
		);
	}
#endif
	return BestCandidate;
}

TArray<FCandidateEntry> ULockOnFunctionLibrary::GatherCandidates(const AActor* TargetingActor, const float TargetingRadius, const float MaxLockOnDistance, const bool bRequireLineOfSight)
{
	if (!TargetingActor) { return {}; }

	// Sphere overlap to collect nearby actors cheaply.
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TargetingActor);

	TargetingActor->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		TargetingActor->GetActorLocation(),
		FQuat::Identity,
		ECC_Target,
		FCollisionShape::MakeSphere(TargetingRadius),
		Params
	);

	TArray<FCandidateEntry> Result;
	Result.Reserve(Overlaps.Num());

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* PotentialTarget = Overlap.GetActor();
		if (!IsTargetValid(PotentialTarget)) { continue; }

		// Hard distance cap (the overlap radius may be slightly larger).
		const float Dist = FVector::Dist(TargetingActor->GetActorLocation(), PotentialTarget->GetActorLocation());
		if (Dist > MaxLockOnDistance) { continue; }

		// Line-of-sight check.
		if (bRequireLineOfSight && !HasLineOfSight(TargetingActor, PotentialTarget)) { continue; }

		Result.Add(FCandidateEntry(PotentialTarget, Dist));
	}

	return Result;
}

bool ULockOnFunctionLibrary::IsTargetValid(const AActor* Target)
{
	if (!IsValid(Target))
	{
		return false;
	}
	if (Target->Implements<UCombatInterface>())
	{
		if (!ICombatInterface::IsAlive(Target))
		{
			return false;
		}
	}
	return true;
}

bool ULockOnFunctionLibrary::HasLineOfSight(const AActor* TargetingActor, const AActor* Target)
{
	if (!IsValid(TargetingActor) || !IsValid(Target))
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TargetingActor);
	Params.AddIgnoredActor(Target);

	// A blocking hit means something is in the way.
	return !Target->GetWorld()->LineTraceSingleByChannel(Hit, TargetingActor->GetActorLocation(), Target->GetActorLocation(), ECC_Visibility, Params);
}
