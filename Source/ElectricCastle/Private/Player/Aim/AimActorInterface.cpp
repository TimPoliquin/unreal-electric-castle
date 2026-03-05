// Copyright Alien Shores


#include "Player/Aim/AimActorInterface.h"


// Add default functionality here for any IAimActorInterface functions that are not pure virtual.
UAimController* IAimActorInterface::GetAimController(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UAimActorInterface>())
	{
		return Execute_GetAimController(Object);
	}
	return nullptr;
}
