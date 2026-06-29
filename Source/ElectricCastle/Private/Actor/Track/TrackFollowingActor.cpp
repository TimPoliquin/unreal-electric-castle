// Copyright Alien Shores


#include "Actor/Track/TrackFollowingActor.h"

#include "Actor/Track/TrackFollowingComponent.h"


UTrackFollowingComponent* ITrackFollowingActor::GetTrackFollowingComponent(const UObject* Actor)
{
	return Actor && Actor->Implements<UTrackFollowingActor>() ? Execute_GetTrackFollowingComponent(Actor) : nullptr;
}
