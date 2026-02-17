// Copyright Alien Shores


#include "Actor/TriggerInterface.h"


// Add default functionality here for any ITriggerInterface functions that are not pure virtual.
void ITriggerInterface::Trigger(UObject* TriggerActor, const bool InActivate)
{
	if (IsValid(TriggerActor) && TriggerActor->Implements<UTriggerInterface>())
	{
		Execute_OnTrigger(TriggerActor, InActivate);
	}
}
