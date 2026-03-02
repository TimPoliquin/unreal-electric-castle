// Copyright Alien Shores


#include "Actor/Cinematic/CinematicPlaybackActorInterface.h"


// Add default functionality here for any ICinematicPlaybackActorInterface functions that are not pure virtual.
void ICinematicPlaybackActorInterface::Start(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UCinematicPlaybackActorInterface>())
	{
		Execute_Start(Actor);
	}
}

void ICinematicPlaybackActorInterface::End(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UCinematicPlaybackActorInterface>())
	{
		Execute_End(Actor);
	}
}
