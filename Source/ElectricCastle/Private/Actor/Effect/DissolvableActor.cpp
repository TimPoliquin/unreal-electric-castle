// Copyright Alien Shores


#include "Actor/Effect/DissolvableActor.h"


// Add default functionality here for any IDissolvableActor functions that are not pure virtual.
void IDissolvableActor::Dissolve(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UDissolvableActor>())
	{
		Execute_Dissolve(Object);
	}
}
