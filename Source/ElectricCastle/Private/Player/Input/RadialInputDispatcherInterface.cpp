// Copyright Alien Shores


#include "Player/Input/RadialInputDispatcherInterface.h"


// Add default functionality here for any IRadialInputDispatcherInterface functions that are not pure virtual.
URadialUIInputComponent* IRadialInputDispatcherInterface::GetRadialUIInputComponent(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<URadialInputDispatcherInterface>())
	{
		return Execute_GetRadialUIInputComponent(Object);
	}
	return nullptr;
}
