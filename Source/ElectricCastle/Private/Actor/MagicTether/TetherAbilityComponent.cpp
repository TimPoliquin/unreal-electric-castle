// Copyright Alien Shores


#include "Actor/MagicTether/TetherAbilityComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/MagicTether/TetherAbilityTypes.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UTetherAbilityComponent::UTetherAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

bool UTetherAbilityComponent::ModifyInputMovementByTetherLimits(
	const FVector& InDirection,
	const float InScale,
	FVector& OutDirection,
	float& OutScale) const
{
	if (!IsTethered() || !OwnerActor || !TargetActor.IsValid())
	{
		OutDirection = InDirection;
		OutScale = InScale;
		return false;
	}

	const FVector CurrentLoc = OwnerActor->GetActorLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();
	const float CurrentDist = FVector::Distance(CurrentLoc, TargetLoc);

	if (FMath::IsNearlyZero(InScale) || InDirection.IsNearlyZero())
	{
		OutDirection = InDirection;
		OutScale = InScale;
		return false;
	}

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector DirNorm = InDirection.GetSafeNormal();
	const FVector TetherDir = (TargetLoc - CurrentLoc).GetSafeNormal();

	// --- Decompose movement ---
	FVector MovementDelta = DirNorm * InScale * DeltaTime;
	const float MoveAlong = FVector::DotProduct(MovementDelta, TetherDir);
	FVector RadialDelta = TetherDir * MoveAlong;
	const FVector TangentialDelta = FVector::VectorPlaneProject(MovementDelta, TetherDir);

	float ScaleFactor = 1.f;
	const float ProposedDist = FVector::Distance(CurrentLoc + MovementDelta, TargetLoc);

	// --- MAX DISTANCE CLAMP (moving away) ---
	if (MoveAlong < 0.f && ProposedDist > MaxTetherLength)
	{
		const float DistToMax = MaxTetherLength - CurrentDist;
		const float MoveAway = -MoveAlong;

		if (MoveAway > KINDA_SMALL_NUMBER)
		{
			ScaleFactor = FMath::Clamp(DistToMax / MoveAway, 0.f, 1.f);
		}
	}

	// --- SOFT INNER RADIUS (scale inward radial only) ---
	if (MoveAlong > 0.f && bUseSoftZone)
	{
		const float SoftRadius = MinTetherLength + SoftZoneWidth;

		if (CurrentDist < SoftRadius)
		{
			const float DistIntoSoft = SoftRadius - CurrentDist;
			const float Alpha = 1.f - FMath::Clamp(DistIntoSoft / SoftZoneWidth, 0.f, 1.f);
			RadialDelta *= Alpha;
		}
	}

	// --- HARD MIN CLAMP (only when pinned, near Min) ---
	if (bTargetWasBlockedLastFrame && MoveAlong > 0.f)
	{
		constexpr float ClampEpsilon = 5.f; // small band near Min

		if (CurrentDist <= MinTetherLength + ClampEpsilon)
		{
			RadialDelta = FVector::ZeroVector;
		}
	}

	// --- Recompose movement ---
	MovementDelta = TangentialDelta + RadialDelta;

	// --- Apply max-distance scaling ---
	MovementDelta *= ScaleFactor;

	const float NewScale = MovementDelta.Size() / DeltaTime;

	if (NewScale < KINDA_SMALL_NUMBER)
	{
		OutDirection = FVector::ZeroVector;
		OutScale = 0.f;
	}
	else
	{
		OutDirection = MovementDelta / (NewScale * DeltaTime);
		OutScale = NewScale;
	}

	return true;
}

void UTetherAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();

	if (OwnerActor)
	{
		AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	}
}

void UTetherAbilityComponent::AttachTarget(AActor* NewTarget)
{
	// If client, forward to server
	if (!GetOwner()->HasAuthority())
	{
		ServerAttachTarget(NewTarget);
		return;
	}

	TargetActor = NewTarget;

	if (TargetActor.IsValid() && OwnerActor)
	{
		TargetActor->AddActorWorldOffset(FVector::UpVector * 20.f);
		CurrentTetherLength = FVector::Distance(
			OwnerActor->GetActorLocation(),
			TargetActor->GetActorLocation()
		);
		PreviousOwnerLocation = OwnerActor->GetActorLocation();
	}
}

void UTetherAbilityComponent::SnapTargetToForwardStart()
{
	if (!OwnerActor || !TargetActor.IsValid())
	{
		return;
	}

	const FVector OwnerLoc = OwnerActor->GetActorLocation();
	const FVector Forward = OwnerActor->GetActorForwardVector().GetSafeNormal();

	// Distance you want the object to start at.
	// Usually MinTetherLength, but you can expose this as a variable.
	const float StartDistance = FVector::Distance(OwnerLoc, TargetActor->GetActorLocation());

	const FVector DesiredLoc = OwnerLoc + Forward * StartDistance + FVector::UpVector * 20.f;

	// Sweep the target to the desired location
	FHitResult Hit;
	TargetActor->SetActorLocation(DesiredLoc, true, &Hit);

	FVector FinalLoc = TargetActor->GetActorLocation();

	// If sweep hit something, slide along the surface
	if (Hit.bBlockingHit)
	{
		const FVector Normal = Hit.Normal.GetSafeNormal();
		FVector SlideDelta = FVector::VectorPlaneProject(DesiredLoc - FinalLoc, Normal);
		SlideDelta.Z = 0.f;
		if (!SlideDelta.IsNearlyZero())
		{
			FHitResult SlideHit;
			TargetActor->SetActorLocation(FinalLoc + SlideDelta, true, &SlideHit);
			FinalLoc = TargetActor->GetActorLocation();
		}
	}

	// Update tether length
	CurrentTetherLength = FVector::Distance(OwnerLoc, FinalLoc);

	// Important: first frame should not be considered "blocked"
	bTargetWasBlockedLastFrame = false;

	// Also update previous owner location so the first UpdateTether() doesn't think we moved
	PreviousOwnerLocation = OwnerLoc;
}

void UTetherAbilityComponent::ServerAttachTarget_Implementation(AActor* NewTarget)
{
	AttachTarget(NewTarget);
}

void UTetherAbilityComponent::DetachTarget()
{
	if (!GetOwner()->HasAuthority())
	{
		ServerDetachTarget();
		return;
	}

	TargetActor = nullptr;
	CurrentTetherLength = 0.f;
}

void UTetherAbilityComponent::ServerDetachTarget_Implementation()
{
	DetachTarget();
}

void UTetherAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only the server runs the tether logic
	if (GetOwner()->HasAuthority() && TargetActor.IsValid())
	{
		UpdateTether(DeltaTime);
	}
	if (OwnerActor)
	{
		PreviousOwnerLocation = OwnerActor->GetActorLocation();
	}
}

void UTetherAbilityComponent::UpdateTether(float DeltaTime)
{
	if (!OwnerActor || !TargetActor.IsValid())
	{
		return;
	}

	const FVector OwnerLoc = OwnerActor->GetActorLocation();
	FVector CurrentLoc = TargetActor->GetActorLocation();
	const FVector OwnerDelta = OwnerLoc - PreviousOwnerLocation;

	// If the owner didn't move, preserve blocked state and just do push-out
	const bool bOwnerMoved = !OwnerDelta.IsNearlyZero();
	bool bBlockedThisFrame = false;

	if (bOwnerMoved)
	{
		FVector RemainingDelta = OwnerDelta;

		// Max tether length check BEFORE movement
		const FVector FirstTargetLoc = CurrentLoc + OwnerDelta;
		const float ProposedDist = FVector::Distance(OwnerLoc, FirstTargetLoc);
		if (ProposedDist > MaxTetherLength + KINDA_SMALL_NUMBER)
		{
			OnTetherLimitExceeded.Broadcast(
				FTetherLimitExceededPayload(OwnerActor, TargetActor.Get(), this, ProposedDist, MaxTetherLength)
			);
			return;
		}

		for (int32 Iter = 0; Iter < RecalcMaxIterations && !RemainingDelta.IsNearlyZero(); ++Iter)
		{
			const FVector AttemptLoc = CurrentLoc + RemainingDelta;

			FHitResult Hit;
			TargetActor->SetActorLocation(AttemptLoc, true, &Hit);

			const FVector NewLoc = TargetActor->GetActorLocation();
			const FVector MovedDelta = NewLoc - CurrentLoc;
			const bool bMoved = !NewLoc.Equals(CurrentLoc, 0.01f);
			const bool bSweepBlocked = Hit.bBlockingHit && !bMoved;

			if (!Hit.bBlockingHit && bMoved)
			{
				CurrentLoc = NewLoc;
				break;
			}

			RemainingDelta -= MovedDelta;

			if (RemainingDelta.IsNearlyZero())
			{
				CurrentLoc = NewLoc;
				break;
			}

			// Slide
			const FVector Normal = Hit.Normal.GetSafeNormal();
			const FVector SlideDelta = FVector::VectorPlaneProject(RemainingDelta, Normal);

			if (SlideDelta.IsNearlyZero())
			{
				CurrentLoc = NewLoc;
				break;
			}

			const FVector SlideStart = NewLoc;
			const FVector SlideTarget = SlideStart + SlideDelta;

			FHitResult SlideHit;
			TargetActor->SetActorLocation(SlideTarget, true, &SlideHit);

			const FVector AfterSlideLoc = TargetActor->GetActorLocation();
			const bool bSlideMoved = !AfterSlideLoc.Equals(NewLoc, 0.01f);
			const bool bSlideBlocked = SlideHit.bBlockingHit && !bSlideMoved;

			if (bSweepBlocked || bSlideBlocked)
			{
				bBlockedThisFrame = true;
			}

			CurrentLoc = AfterSlideLoc;
			RemainingDelta -= SlideDelta;
		}
	}

	// --- Push-out if inside minimum radius ---
	if (bPushOut && !(bBlockedThisFrame || bTargetWasBlockedLastFrame))
	{
		if (float CurrentDist = FVector::Distance(OwnerLoc, CurrentLoc); CurrentDist < MinTetherLength + SoftZoneWidth)
		{
			const float DistInside = (MinTetherLength + SoftZoneWidth) - CurrentDist;

			// Tunable push strength (units per second)
			const float PushAmount = FMath::Min(DistInside, PushStrength * DeltaTime);

			FVector OutwardDir = (CurrentLoc - OwnerLoc).GetSafeNormal();
			OutwardDir.Z = 0.f;
			const FVector PushDelta = OutwardDir * PushAmount;
			FHitResult PushHit;
			const FVector PushTarget = CurrentLoc + PushDelta;
			TargetActor->SetActorLocation(PushTarget, true, &PushHit);

			FVector AfterPushLoc = TargetActor->GetActorLocation();

			if (PushHit.bBlockingHit)
			{
				TargetActor->SetActorLocation(CurrentLoc, false, &PushHit);
			}

			CurrentLoc = AfterPushLoc;
			CurrentDist = FVector::Distance(OwnerLoc, CurrentLoc);
		}
	}

	CurrentTetherLength = FVector::Distance(OwnerLoc, CurrentLoc);

	bTargetWasBlockedLastFrame = bBlockedThisFrame;
	PreviousOwnerLocation = OwnerLoc;
}

void UTetherAbilityComponent::OnRep_TargetActor()
{
	// Optional: spawn VFX, update cable, etc.
}

void UTetherAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTetherAbilityComponent, TargetActor);
	DOREPLIFETIME(UTetherAbilityComponent, CurrentTetherLength);
}
