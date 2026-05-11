// Copyright Alien Shores


#include "AI/Alert/AIAlertActor.h"

#include "AI/Alert/AIAlertComponent.h"


// Add default functionality here for any IAIAlertActor functions that are not pure virtual.
UAIAlertComponent* IAIAlertActor::GetAIAlertComponent(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UAIAlertActor>())
	{
		return Execute_GetAIAlertComponent(Actor);
	}
	return nullptr;
}
