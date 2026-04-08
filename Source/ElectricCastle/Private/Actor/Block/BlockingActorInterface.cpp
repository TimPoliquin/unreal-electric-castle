// Copyright Alien Shores


#include "Actor/Block/BlockingActorInterface.h"


// Add default functionality here for any IBlockingActorInterface functions that are not pure virtual.
UBlockController* IBlockingActorInterface::GetBlockController(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UBlockingActorInterface>())
	{
		return Execute_GetBlockController(Object);
	}
	return nullptr;
}
