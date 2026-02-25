// Copyright Alien Shores


#include "Actor/Cinematic/CinematicHandlerInterface.h"


bool ICinematicHandlerInterface::IsCinematicHandler(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UCinematicHandlerInterface>();
}

// Add default functionality here for any ICinematicHandlerInterface functions that are not pure virtual.
UCinematicHandlerComponent* ICinematicHandlerInterface::GetCinematicHandlerComponent(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UCinematicHandlerInterface>())
	{
		return Execute_GetCinematicHandlerComponent(Object);
	}
	return nullptr;
}
