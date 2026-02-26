// Copyright Alien Shores


#include "Actor/TriggerInterface.h"


// Add default functionality here for any ITriggerInterface functions that are not pure virtual.
void ITriggerInterface::Trigger(UObject* TriggerActor, const bool InActivate)
{
	if (IsTrigger(TriggerActor))
	{
		Execute_OnTrigger(TriggerActor, InActivate);
	}
}

bool ITriggerInterface::IsTrigger(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UTriggerInterface>())
	{
		return true;
	}
	return false;
}

bool ITriggerInterface::IsTriggered(const UObject* Object)
{
	if (IsTrigger(Object))
	{
		return Execute_IsTriggered(Object);
	}
	return false;
}
