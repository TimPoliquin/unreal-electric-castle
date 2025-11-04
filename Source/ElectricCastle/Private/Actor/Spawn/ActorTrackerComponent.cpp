// Copyright Alien Shores


#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Actor/Spawn/TrackableInterface.h"


UActorTrackerComponent::UActorTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorTrackerComponent::Track(AActor* Actor)
{
	if (IsValid(Actor))
	{
		const int32 OldCount = Actors.Num();
		Actors.AddUnique(Actor);
		if (Actor->Implements<UTrackableInterface>())
		{
			Cast<ITrackableInterface>(Actor)->GetStopTrackingDelegate().AddUniqueDynamic(this, &UActorTrackerComponent::OnTrackedActorDestroyed);
		}
		else
		{
			Actor->OnDestroyed.AddUniqueDynamic(this, &UActorTrackerComponent::OnTrackedActorDestroyed);
		}
		OnCountChanged.Broadcast(FOnActorTrackerCountChangedPayload(GetOwner(), OldCount, Actors.Num()));
	}
}

void UActorTrackerComponent::OnTrackedActorDestroyed(AActor* DestroyedActor)
{
	const int32 OldCount = Actors.Num();
	Actors.RemoveAll([DestroyedActor](const AActor* TrackedActor)
	{
		return TrackedActor == DestroyedActor || !IsValid(TrackedActor);
	});
	OnCountChanged.Broadcast(FOnActorTrackerCountChangedPayload(GetOwner(), OldCount, Actors.Num()));
}
