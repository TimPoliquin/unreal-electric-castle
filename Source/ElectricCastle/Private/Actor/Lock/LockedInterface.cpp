// Copyright Alien Shores


#include "Actor/Lock/LockedInterface.h"


// Add default functionality here for any ILockedInterface functions that are not pure virtual.
void ILockedInterface::Unlock(UObject* Gate)
{
	if (IsValid(Gate) && Gate->Implements<ULockedInterface>())
	{
		Execute_Unlock(Gate);
	}
}
