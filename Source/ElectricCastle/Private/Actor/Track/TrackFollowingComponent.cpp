// Copyright Alien Shores


#include "Actor/Track/TrackFollowingComponent.h"

#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


UTrackFollowingComponent::UTrackFollowingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTrackFollowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bActive && Track)
	{
		MoveAlongTrack(DeltaTime);
	}
}

// Called when the game starts
void UTrackFollowingComponent::BeginPlay()
{
	Super::BeginPlay();
	DistanceAlongTrack = Track ? FMath::Clamp(StartingDistanceAlongTrack, 0, Track->GetSplineLength()) : StartingDistanceAlongTrack;
	if (bActive)
	{
		MoveAlongTrack(0);
	}
}

void UTrackFollowingComponent::MoveAlongTrack(const float DeltaTime)
{
	if (!Track)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No track set on track component!"), *GetOwner()->GetName())
		return;
	}
	const int32 DirectionalMultiplier = Direction == ETrackDirection::Forward ? 1 : -1;
	DistanceAlongTrack = FMath::Clamp(DistanceAlongTrack + MovementSpeed * DeltaTime * DirectionalMultiplier, 0, Track->GetSplineLength());
	const FTransform SplineTransform = Track->GetTransformAtDistanceAlongSpline(DistanceAlongTrack, ESplineCoordinateSpace::World);
	GetOwner()->SetActorLocation(SplineTransform.GetLocation());
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Track progress: %f/%f"), *GetOwner()->GetName(), *GetName(), DistanceAlongTrack, Track->GetSplineLength());
	}
	if (IsAtStartOfTrack() || IsAtEndOfTrack())
	{
		HandleTrackEnd();
	}
}

void UTrackFollowingComponent::HandleTrackEnd()
{
	if (LoopMode > ETrackLoopMode::UniDirectional)
	{
		if (Direction == ETrackDirection::Forward)
		{
			Direction = ETrackDirection::Reverse;
		}
		else
		{
			Direction = ETrackDirection::Forward;
		}
	}
	OnTrackCompleted.Broadcast(GetOwner(), Track, Track->GetSplineLength() - DistanceAlongTrack);
	if (LoopMode == ETrackLoopMode::BiDirectionalAuto)
	{
		if (LoopDelay > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LoopDelayTimer,
				[this]()
				{
					Activate();
				},
				LoopDelay,
				false
			);
		}
	}
	else
	{
		Deactivate();
	}
}

bool UTrackFollowingComponent::IsAtStartOfTrack() const
{
	return Direction == ETrackDirection::Reverse && FMath::IsNearlyZero(DistanceAlongTrack);
}

bool UTrackFollowingComponent::IsAtEndOfTrack() const
{
	return Direction == ETrackDirection::Forward && DistanceAlongTrack >= Track->GetSplineLength();
}

void UTrackFollowingComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTrackFollowingComponent, bActive)
}

void UTrackFollowingComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTrackFollowingComponent, StartingDistanceAlongTrack))
	{
		if (Track)
		{
			GetOwner()->SetActorLocation(Track->GetTransformAtDistanceAlongSpline(StartingDistanceAlongTrack, ESplineCoordinateSpace::World).GetLocation());
		}
	}
}

USplineComponent* UTrackFollowingComponent::GetTrack() const
{
	return Track;
}

void UTrackFollowingComponent::SetTrack(USplineComponent* InTrack)
{
	Track = InTrack;
}

void UTrackFollowingComponent::Activate(const bool bReset)
{
	Super::Activate(bReset);
	bActive = true;
}

void UTrackFollowingComponent::Deactivate()
{
	Super::Deactivate();
	bActive = false;
}

float UTrackFollowingComponent::GetTrackDistance() const
{
	return DistanceAlongTrack;
}

void UTrackFollowingComponent::SetDirection(const ETrackDirection InDirection)
{
	Direction = InDirection;
}

void UTrackFollowingComponent::ChangeSpeed(const float InMovementSpeed)
{
	MovementSpeed = InMovementSpeed;
}

void UTrackFollowingComponent::SetStartingDistanceAlongTrack(const float InStartingDistanceAlongTrack)
{
	StartingDistanceAlongTrack = InStartingDistanceAlongTrack;
	DistanceAlongTrack = InStartingDistanceAlongTrack;
}
