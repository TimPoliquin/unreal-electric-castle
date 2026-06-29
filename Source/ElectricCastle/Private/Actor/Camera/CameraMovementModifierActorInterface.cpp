// Copyright Alien Shores


#include "Actor/Camera/CameraMovementModifierActorInterface.h"


// Add default functionality here for any ICameraMovementModifierActorInterface functions that are not pure virtual.
UCameraMovementModifier* ICameraMovementModifierActorInterface::GetCameraMovementModifier(UObject* Actor)
{
	return IsValid(Actor) && Actor->Implements<UCameraMovementModifierActorInterface>() ? Execute_GetCameraMovementModifier(Actor) : nullptr;
}
