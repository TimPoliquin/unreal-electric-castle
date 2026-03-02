// Copyright Alien Shores


#include "Actor/Horde/HordeModeActorInterface.h"


// Add default functionality here for any IHordeModeActorInterface functions that are not pure virtual.
UHordeModeComponent* IHordeModeActorInterface::GetHordeModeComponent(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UHordeModeActorInterface>())
	{
		return Execute_GetHordeModeComponent(Object);
	}
	return nullptr;
}
