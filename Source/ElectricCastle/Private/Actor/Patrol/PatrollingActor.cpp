// Copyright Alien Shores


#include "Actor/Patrol/PatrollingActor.h"

#include "Actor/Patrol/PatrolComponent.h"


// Add default functionality here for any IPatrollingActor functions that are not pure virtual.
UPatrolComponent* IPatrollingActor::GetPatrolComponent(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UPatrollingActor>())
	{
		return Execute_GetPatrolComponent(Actor);
	}
	return nullptr;
}

bool IPatrollingActor::CanPatrol(const UObject* Actor)
{
	if (const UPatrolComponent* PatrolComponent = GetPatrolComponent(Actor))
	{
		return PatrolComponent->CanPatrol();
	}
	return false;
}
