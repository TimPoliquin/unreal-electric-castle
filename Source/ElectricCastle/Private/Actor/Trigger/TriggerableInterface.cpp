// Copyright Alien Shores


#include "Actor/Trigger/TriggerableInterface.h"


// Add default functionality here for any ITriggerInterface functions that are not pure virtual.
void ITriggerableInterface::Trigger(UObject* TriggerActor, const bool InActivate)
{
	if (IsTrigger(TriggerActor))
	{
		Execute_OnTrigger(TriggerActor, InActivate);
	}
}

bool ITriggerableInterface::IsTrigger(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UTriggerableInterface>())
	{
		return true;
	}
	return false;
}

bool ITriggerableInterface::IsTriggered(const UObject* Object)
{
	if (IsTrigger(Object))
	{
		return Execute_IsTriggered(Object);
	}
	return false;
}
