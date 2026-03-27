// Copyright Alien Shores


#include "Player/LockOn/LockOnActor.h"


// Add default functionality here for any ILockOnActor functions that are not pure virtual.
ULockOnController* ILockOnActor::GetLockOnController(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<ULockOnActor>())
	{
		return Execute_GetLockOnController(Actor);
	}
	return nullptr;
}
