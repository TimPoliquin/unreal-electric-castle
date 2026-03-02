// Copyright Alien Shores


#include "Actor/Lock/LockInterface.h"


void ILockInterface::Lock(UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<ULockInterface>())
	{
		Execute_Lock(Actor);
	}
}

// Add default functionality here for any ILockedInterface functions that are not pure virtual.
void ILockInterface::Unlock(UObject* Gate)
{
	if (IsValid(Gate) && Gate->Implements<ULockInterface>())
	{
		Execute_Unlock(Gate);
	}
}
