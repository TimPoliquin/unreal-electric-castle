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
	FVector MovementDelta = DirNorm * InScale * DeltaTime;

	const FVector TetherDir = (TargetLoc - CurrentLoc).GetSafeNormal();
	const float MoveAlongTether = FVector::DotProduct(MovementDelta, TetherDir);

	const float ProposedDist = FVector::Distance(CurrentLoc + MovementDelta, TargetLoc);

	float ScaleFactor = 1.0f;

	// --- MAX DISTANCE CLAMP ---
	if (MoveAlongTether < 0.f && ProposedDist > MaxTetherLength)
	{
		const float DistToMax = MaxTetherLength - CurrentDist;
		const float MoveAway = -MoveAlongTether;

		if (MoveAway > KINDA_SMALL_NUMBER)
		{
			ScaleFactor = FMath::Clamp(DistToMax / MoveAway, 0.f, 1.f);
		}
	}

	// --- MIN DISTANCE CLAMP ---
	// Only block inward movement if the target was blocked last frame.
	if (bTargetWasBlockedLastFrame && MoveAlongTether > 0.f)
	{
		if (CurrentDist <= MinTetherLength + KINDA_SMALL_NUMBER)
		{
			OutDirection = FVector::ZeroVector;
			OutScale = 0.f;
			return true;
		}
	}

	// No clamp needed
	if (FMath::IsNearlyEqual(ScaleFactor, 1.0f))
	{
		OutDirection = InDirection;
		OutScale = InScale;
		return false;
	}

	// Apply clamp
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
	const FVector OwnerDelta = OwnerLoc - PreviousOwnerLocation;

	// If the owner didn't move, preserve blocked state
	if (OwnerDelta.IsNearlyZero())
	{
		return;
	}

	const FVector StartObjLoc = TargetActor->GetActorLocation();
	FVector RemainingDelta = OwnerDelta;
	FVector CurrentLoc = StartObjLoc;

	// Max tether length check BEFORE movement
	const FVector FirstTargetLoc = StartObjLoc + OwnerDelta;
	const float ProposedDist = FVector::Distance(OwnerLoc, FirstTargetLoc);
	if (ProposedDist > MaxTetherLength + KINDA_SMALL_NUMBER)
	{
		OnTetherLimitExceeded.Broadcast(
			FTetherLimitExceededPayload(OwnerActor, TargetActor.Get(), this, ProposedDist, MaxTetherLength)
		);
		return;
	}

	constexpr int32 MaxIterations = 3;
	bool bBlockedThisFrame = false;

	for (int32 Iter = 0; Iter < MaxIterations && !RemainingDelta.IsNearlyZero(); ++Iter)
	{
		const FVector AttemptLoc = CurrentLoc + RemainingDelta;

		FHitResult Hit;
		TargetActor->SetActorLocation(AttemptLoc, true, &Hit);

		const FVector NewLoc = TargetActor->GetActorLocation();
		const FVector MovedDelta = NewLoc - CurrentLoc;
		const bool bMoved = !NewLoc.Equals(CurrentLoc, 0.01f);

		// Sweep-blocked only if sweep hit AND no movement occurred
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

		// Slide-blocked only if slide hit AND no movement occurred
		const bool bSlideBlocked = SlideHit.bBlockingHit && !bSlideMoved;

		if (bSweepBlocked || bSlideBlocked)
		{
			bBlockedThisFrame = true;
		}

		CurrentLoc = AfterSlideLoc;
		RemainingDelta -= SlideDelta;
	}

	// --- NEW: Push-out if inside minimum radius ---

	if (const float CurrentDist = FVector::Distance(OwnerLoc, CurrentLoc); CurrentDist < MinTetherLength)
	{
		const float DistInside = MinTetherLength - CurrentDist;

		// Tunable push strength (units per second)
		constexpr float PushStrength = 50.f;

		const float PushAmount = FMath::Min(DistInside + MinTetherBufferLength, PushStrength * DeltaTime);
		const FVector OutwardDir = (CurrentLoc - OwnerLoc).GetSafeNormal();
		FVector PushDelta = OutwardDir * PushAmount;

		// Sweep the push
		FHitResult PushHit;
		const FVector PushTarget = CurrentLoc + PushDelta;
		TargetActor->SetActorLocation(PushTarget, true, &PushHit);

		FVector AfterPushLoc = TargetActor->GetActorLocation();

		// Slide if needed
		if (PushHit.bBlockingHit)
		{
			const FVector Normal = PushHit.Normal.GetSafeNormal();
			const FVector SlideDelta = FVector::VectorPlaneProject(PushDelta, Normal);

			if (!SlideDelta.IsNearlyZero())
			{
				FHitResult SlideHit;
				TargetActor->SetActorLocation(AfterPushLoc + SlideDelta, true, &SlideHit);
				AfterPushLoc = TargetActor->GetActorLocation();
			}
		}

		CurrentLoc = AfterPushLoc;
	}

	CurrentTetherLength = FVector::Distance(OwnerLoc, CurrentLoc);

	// Update blocked state for next frame
	bTargetWasBlockedLastFrame = bBlockedThisFrame;
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
