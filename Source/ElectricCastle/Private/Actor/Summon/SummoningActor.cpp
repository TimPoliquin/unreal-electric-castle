// Copyright Alien Shores


#include "Actor/Summon/SummoningActor.h"


// Add default functionality here for any ISummoningActor functions that are not pure virtual.
USummonComponent* ISummoningActor::GetSummonComponent(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<USummoningActor>())
	{
		return Execute_GetSummonComponent(Actor);
	}
	return nullptr;
}
