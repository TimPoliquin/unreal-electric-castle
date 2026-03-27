// Copyright Alien Shores


#include "Actor/Attack/Component/AttackWindowManagerActor.h"

#include "Actor/Attack/Component/AttackWindowManager.h"


// Add default functionality here for any IAttackWindowManagerActor functions that are not pure virtual.
UAttackWindowManager* IAttackWindowManagerActor::GetAttackWindowManager(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UAttackWindowManagerActor>())
	{
		return Execute_GetAttackWindowManager(Actor);
	}
	return nullptr;
}
