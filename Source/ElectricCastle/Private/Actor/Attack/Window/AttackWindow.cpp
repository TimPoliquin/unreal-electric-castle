// Copyright Alien Shores


#include "Actor/Attack/Window/AttackWindow.h"

#include "KismetTraceUtils.h"

#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerComponent.h"

void UAttackWindow::SetOwner(AActor* InOwner)
{
	Owner = InOwner;
}

void UAttackWindow::SetDuration(const float InDuration)
{
	Duration = InDuration;
	Elapsed = 0.f;
	PreviousAlpha = 0.f;
}

void UAttackWindow::SweepForHits(const float DeltaTime, TArray<FHitResult>& OutHits)
{
	if (!Owner.IsValid())
	{
		return;
	}
	Elapsed = FMath::Clamp(Elapsed + DeltaTime, 0.f, Duration);
	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);
	TArray<FHitResult> CurrentHits;
	for (FRuntimeVectorCurve& Curve : SweepCurves)
	{
		SweepCurveForHits(Curve, Alpha, CurrentHits);
	}
	if (!bAllowRepeatHits)
	{
		for (const FHitResult& Hit : CurrentHits)
		{
			if (!HitActors.ContainsByPredicate([Hit](const TWeakObjectPtr<AActor> Actor) { return Actor.Get() == Hit.GetActor(); }))
			{
				OutHits.Add(Hit);
			}
		}
	}
	else
	{
		OutHits = CurrentHits;
	}
	PreviousAlpha = Alpha;
}

FVector UAttackWindow::GetOffsetLocation(const FVector& InLocation) const
{
	FVector Origin;
	if (OriginSocketTag.IsValid())
	{
		if (USocketManagerComponent* SocketManagerComponent = ISocketManagerActor::GetSocketManagerComponent(Owner.Get()))
		{
			Origin = SocketManagerComponent->GetSocketLocation(OriginSocketTag);
		}
		else
		{
			Origin = Owner->GetActorLocation();
		}
	}
	else
	{
		Origin = Owner->GetActorLocation();
	}
	const FRotator Rotation = Owner->GetActorRotation();
	return Origin + Rotation.RotateVector(Offset) + Rotation.RotateVector(InLocation);
}

void UAttackWindow::SweepCurveForHits(const FRuntimeVectorCurve& InSweepCurve, const float Alpha, TArray<FHitResult>& OutHits) const
{
	const FVector StartLocation = FMath::IsNearlyZero(PreviousAlpha) ? GetOffsetLocation(InSweepCurve.GetValue(0.f)) : GetOffsetLocation(InSweepCurve.GetValue(PreviousAlpha));
	const FVector EndLocation = GetOffsetLocation(InSweepCurve.GetValue(Alpha));
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner.Get());
	Owner->GetWorld()->SweepMultiByChannel(
		OutHits,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		SweepChannel,
		FCollisionShape::MakeCapsule(SweepRadius, SweepHalfHeight),
		Params
	);
	if (bDebug)
	{
		DrawDebugCapsuleTraceMulti(Owner->GetWorld(), StartLocation, EndLocation, SweepRadius, SweepHalfHeight, EDrawDebugTrace::ForDuration, false, OutHits, FColor::White, FColor::Red, 2.f);
	}
}
