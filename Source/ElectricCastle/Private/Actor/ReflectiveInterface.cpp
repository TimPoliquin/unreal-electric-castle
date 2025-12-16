// Copyright Alien Shores


#include "Actor/ReflectiveInterface.h"


// Add default functionality here for any IReflectiveInterface functions that are not pure virtual.
bool IReflectiveInterface::IsReflective(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UReflectiveInterface>())
	{
		return Execute_IsReflective(Object);
	}
	return false;
}
