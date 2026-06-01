// Copyright Alien Shores


#include "Player/LockOn/LockOnFunctionLibrary.h"

#include "Actor/Significance/WorldSignificanceSubsystem.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Engine/OverlapResult.h"
#include "Interaction/CombatInterface.h"
#include "Tags/ElectricCastleGameplayTags.h"

TArray<const AActor*> ULockOnFunctionLibrary::EMPTY_IGNORE = TArray<const AActor*>();

AActor* ULockOnFunctionLibrary::FindClosestTarget(
	const APlayerController* PlayerController,
	const float LockOnRadius,
	const bool bRequireLineOfSight,
	const bool bDebug
)
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetPawnOrSpectator()))
	{
		return nullptr;
	}
	TArray<FLockOnCandidateEntry> Candidates = GatherCandidates(PlayerController, LockOnRadius, true, EMPTY_IGNORE, bDebug);
	if (Candidates.IsEmpty())
	{
		return nullptr;
	}
	if (bRequireLineOfSight)
	{
		for (const FLockOnCandidateEntry& CandidateEntry : Candidates)
		{
			if (HasLineOfSight(PlayerController->GetPawnOrSpectator(), CandidateEntry.Target))
			{
				return CandidateEntry.Target;
			}
		}
		return nullptr;
	}

	return Candidates[0].Target;
}

AActor* ULockOnFunctionLibrary::SwitchTarget(
	const APlayerController* PlayerController,
	const AActor* CurrentTarget,
	const FVector2D& StickInput,
	const float LockOnRadius,
	const bool bRequireLineOfSight,
	const bool bDebug
)
{
	// 1. Guard against invalid controllers
	if (!PlayerController) { return nullptr; }

	// If there is no current target, we can't "switch" directionally; gather normal candidates
	if (!CurrentTarget)
	{
		TArray<FLockOnCandidateEntry> Candidates = GatherCandidates(PlayerController, LockOnRadius, true, {}, bDebug);
		return Candidates.Num() > 0 ? Candidates[0].Target : nullptr;
	}

	// 2. Project current target to screen space to get our shifting origin
	FVector2D CurrentTargetScreenPos;
	if (!PlayerController->ProjectWorldLocationToScreen(CurrentTarget->GetActorLocation(), CurrentTargetScreenPos))
	{
		// If the current target mysteriously isn't on screen, clear ignores and grab the best overall target
		TArray<FLockOnCandidateEntry> Candidates = GatherCandidates(PlayerController, LockOnRadius, true, {}, bDebug);
		return Candidates.Num() > 0 ? Candidates[0].Target : nullptr;
	}

	// 3. Gather all candidates, explicitly ignoring the current target
	TArray<const AActor*> IgnoreList;
	IgnoreList.Add(CurrentTarget);

	// We pass bScore as false because we are rolling a custom directional scoring pass below
	TArray<FLockOnCandidateEntry> Candidates = GatherCandidates(PlayerController, LockOnRadius, false, IgnoreList, bDebug);
	if (Candidates.IsEmpty())
	{
		return nullptr;
	}

	// --- BEFORE THE LOOP: Get Viewport Sizes once ---
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Convert dimensions to floats
	const float VpX = static_cast<float>(ViewportSizeX);
	const float VpY = static_cast<float>(ViewportSizeY);

	// Normalize Current Target position to a 0.0 - 1.0 range
	const FVector2D NormalizedCurrentTargetPos(CurrentTargetScreenPos.X / VpX, CurrentTargetScreenPos.Y / VpY);
	const FVector2D NormalizedStick = StickInput.GetSafeNormal();
	// ------------------------------------------------

	for (FLockOnCandidateEntry& Candidate : Candidates)
	{
		if (!Candidate.ScreenSpaceData.bIsOnScreen) { continue; }

		// 1. Normalize Candidate Position to 0.0 - 1.0 range
		FVector2D NormalizedCandidatePos(
			Candidate.ScreenSpaceData.ScreenPosition.X / VpX,
			Candidate.ScreenSpaceData.ScreenPosition.Y / VpY
		);

		// 2. Calculate the aspect-ratio-independent direction vector
		const FVector2D DirectionToCandidate = NormalizedCandidatePos - NormalizedCurrentTargetPos;
		const float NormalizedDistance = DirectionToCandidate.Size();

		if (NormalizedDistance < 0.001f) { continue; } // Avoid division by zero

		FVector2D NormalizedDir = DirectionToCandidate / NormalizedDistance;

		// 4. Update the Proximity Weight to use Normalized Distance
		// Since NormalizedDistance ranges roughly from 0.0 to 1.414 (corner to corner),
		// we use a multiplier (like 5.0f) to properly scale down distant targets.

		// 1. Project the target's relative direction onto your normalized stick vector
		const float ComponentAlongStick = FVector2D::DotProduct(DirectionToCandidate, NormalizedStick);

		// If ComponentAlongStick is negative or zero, they are in the opposite hemisphere 
		// of your flick (e.g., you pushed UP, they are BELOW the current target)
		if (ComponentAlongStick <= 0.001f) { continue; }

		// 2. Calculate the perpendicular deviation (how far away they are from the stick's straight line)
		// This is the "Cross Product" magnitude in 2D
		const float PerpendicularDeviation = FMath::Abs(FVector2D::CrossProduct(NormalizedDir, NormalizedStick));

		// 3. Build a smart alignment score: 
		// High progress along the stick axis is rewarded, heavy off-axis drift is penalized
		const float AlignmentScore = ComponentAlongStick * (1.0f - (PerpendicularDeviation * 0.5f));

		// 4. Update the final flick score combining it with proximity
		const float ProximityWeight = 1.0f / (1.0f + (NormalizedDistance * 4.0f));
		Candidate.TotalScore = AlignmentScore * ProximityWeight;
	}
	Candidates.Sort(
		[](const FLockOnCandidateEntry& A, const FLockOnCandidateEntry& B)
		{
			return A.TotalScore > B.TotalScore;
		}
	);
	if (bRequireLineOfSight)
	{
		for (const FLockOnCandidateEntry& Candidate : Candidates)
		{
			if (HasLineOfSight(PlayerController->GetPawnOrSpectator(), Candidate.Target))
			{
				return Candidate.Target;
			}
		}
		return nullptr;
	}

	return Candidates[0].Target;
}

TArray<FLockOnCandidateEntry> ULockOnFunctionLibrary::GatherCandidates(
	const APlayerController* PlayerController,
	const float TargetingRadius,
	const bool bScore,
	const TArray<const AActor*>& IgnoreActors,
	const bool bDebug
)
{
	if (!PlayerController) { return {}; }
	if (UWorldSignificanceSubsystem* SignificanceSubsystem = UWorldSignificanceSubsystem::Get(PlayerController))
	{
		// DEVNOTE - Limit candidates to enemies for now
		if (!SignificanceSubsystem->HasAnySignificantActors(FElectricCastleGameplayTags::Get().Significance_Category_Enemy))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[ULockOnFunctionLibrary] Fast bailout - no significant actors"))
			}
			return {};
		}
	}

	// Sphere overlap to collect nearby actors cheaply.
	const APawn* TargetingPawn = PlayerController->GetPawnOrSpectator();
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TargetingPawn);
	Params.AddIgnoredActors(IgnoreActors);

	TargetingPawn->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		TargetingPawn->GetActorLocation(),
		FQuat::Identity,
		ECC_Target,
		FCollisionShape::MakeSphere(TargetingRadius),
		Params
	);
	TArray<FLockOnCandidateEntry> Result;
	Result.Reserve(Overlaps.Num());
	TSet<AActor*> ProcessedActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* PotentialTarget = Overlap.GetActor();
		if (ProcessedActors.Contains(PotentialTarget) || !IsTargetValid(PotentialTarget)) { continue; }
		ProcessedActors.Add(PotentialTarget);
		FLockOnCandidateEntry CandidateEntry = FLockOnCandidateEntry(PotentialTarget);
		CandidateEntry.Distance = FVector::Dist(TargetingPawn->GetActorLocation(), Overlap.OverlapObjectHandle.GetLocation());
		CandidateEntry.ScreenSpaceData = CalculateScreenSpaceData(PlayerController, PotentialTarget, bScore);
		if (bScore)
		{
			CandidateEntry.DistanceScore = FMath::Pow(FMath::Clamp(1.f - (CandidateEntry.Distance / TargetingRadius), 0.f, 1.f), 2.f);
			CandidateEntry.TotalScore = FMath::Pow(CandidateEntry.DistanceScore, 2.f) + FMath::Pow(CandidateEntry.ScreenSpaceData.ScreenScore, 2);
			// dramatically reduce the score of candidates that are off-screen.
			if (!CandidateEntry.ScreenSpaceData.bIsOnScreen)
			{
				CandidateEntry.TotalScore *= .01f;
			}
		}
		Result.Add(CandidateEntry);
	}
	if (bScore)
	{
		Result.Sort(
			[](const FLockOnCandidateEntry& A, const FLockOnCandidateEntry& B)
			{
				return A.TotalScore > B.TotalScore;
			}
		);
	}
	return Result;
}

FLockOnCandidateScreenSpaceData ULockOnFunctionLibrary::CalculateScreenSpaceData(const APlayerController* PlayerController, const AActor* TargetActor, const bool bScore)
{
	FLockOnCandidateScreenSpaceData Result;
	Result.bIsOnScreen = PlayerController->ProjectWorldLocationToScreen(TargetActor->GetActorLocation(), Result.ScreenPosition);
	if (Result.bIsOnScreen && bScore)
	{
		int32 ViewportSizeX, ViewportSizeY;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		const FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);
		const FVector2D TargetOffset = Result.ScreenPosition - ScreenCenter;
		const float NormalizedX = FMath::Abs(TargetOffset.X) / ScreenCenter.X;
		const float NormalizedY = FMath::Abs(TargetOffset.Y) / ScreenCenter.Y;
		// Find the maximum normalized penalty (will be 0.0 at center, 1.0 at screen edge)
		const float MaxNormalizedOffset = FMath::Max(NormalizedX, NormalizedY);
		Result.ScreenScore = FMath::Clamp(1.f - MaxNormalizedOffset, 0.f, 1.f);
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

bool ULockOnFunctionLibrary::GetCameraDistance(const FVector& TargetLocation, const FVector& CameraLocation, const FVector& CameraForward, float& OutCameraDistance)
{
	// Vector from camera to the potential target
	const FVector ToTarget = TargetLocation - CameraLocation;

	// Optional: If you want to calculate distance regardless of height/pitch, 
	// you could clear the Z components here (e.g., ToTarget.Z = 0; CameraForward.Z = 0; and re-normalize)

	// Project ToTarget onto the Camera Forward vector to find the closest point along the camera line
	const float DirectDistance = FVector::DotProduct(ToTarget, CameraForward);

	// If DirectDistance is negative, the target is actually BEHIND the camera view
	if (DirectDistance < 0.0f) { return false; }

	const FVector ProjectPoint = CameraLocation + (CameraForward * DirectDistance);

	// This gives you the absolute perpendicular distance (in centimeters) from the center line of the camera view
	OutCameraDistance = FVector::Dist(TargetLocation, ProjectPoint);
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
