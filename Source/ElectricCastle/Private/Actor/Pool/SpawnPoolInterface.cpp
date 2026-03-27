// Copyright Alien Shores


#include "Actor/Pool/SpawnPoolInterface.h"


bool ISpawnPoolInterface::IsSpawnPool(const UObject* Pool)
{
	return IsValid(Pool) && Pool->Implements<USpawnPoolInterface>();
}

// Add default functionality here for any ISpawnPoolInterface functions that are not pure virtual.
AActor* ISpawnPoolInterface::AcquireDeferred(UObject* Pool, const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass)
{
	if (IsSpawnPool(Pool))
	{
		return Execute_AcquireDeferred(Pool, WorldContextObject, ActorClass);
	}
	return nullptr;
}

void ISpawnPoolInterface::AcquireFinalize(UObject* Pool, AActor* Actor, const FTransform& SpawnTransform)
{
	if (IsSpawnPool(Pool))
	{
		Execute_AcquireFinalize(Pool, Actor, SpawnTransform);
	}
}

void ISpawnPoolInterface::ReturnToPool(UObject* Pool, AActor* Actor)
{
	if (IsSpawnPool(Pool))
	{
		Execute_ReturnToPool(Pool, Actor);
	}
}
