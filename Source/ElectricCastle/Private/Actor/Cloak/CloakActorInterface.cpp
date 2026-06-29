// Copyright Alien Shores


#include "Actor/Cloak/CloakActorInterface.h"


// Add default functionality here for any ICloakActorInterface functions that are not pure virtual.
UCloakComponent* ICloakActorInterface::GetCloakComponent(const UObject* WorldContextObject)
{
	return IsValid(WorldContextObject) && WorldContextObject->Implements<UCloakActorInterface>()
		       ? Execute_GetCloakComponent(WorldContextObject)
		       : nullptr;
}
