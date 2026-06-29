// Copyright Alien Shores


#include "AI/Targeting/AITargetingActorInterface.h"


// Add default functionality here for any IAITargetingActorInterface functions that are not pure virtual.
UAITargetingComponent* IAITargetingActorInterface::GetAITargetingComponent(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<UAITargetingActorInterface>() ? Execute_GetAITargetingComponent(Object) : nullptr;
}
