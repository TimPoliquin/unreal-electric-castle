// Copyright Alien Shores


#include "SaveInterface.h"


// Add default functionality here for any ISaveInterface functions that are not pure virtual.
bool ISaveInterface::ShouldLoadTransform(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<USaveInterface>())
	{
		return Execute_ShouldLoadTransform(Object);
	}
	return false;
}
