// Copyright Alien Shores


#include "Actor/Pool/PoolableActor.h"

#include "Actor/Pool/PoolManagerComponent.h"


UPoolManagerComponent* IPoolableActor::GetPoolManager(const AActor* Object)
{
	if (IsValid(Object) && Object->Implements<UPoolableActor>())
	{
		return Execute_GetPoolManager(Object);
	}
	return nullptr;
}

// Add default functionality here for any IPoolableActor functions that are not pure virtual.
void IPoolableActor::ReturnToPoolOrDestroy(AActor* Object)
{
	if (!IsValid(Object))
	{
		return;
	}
	if (const UPoolManagerComponent* PoolManagerComponent = GetPoolManager(Object))
	{
		PoolManagerComponent->ReturnToPool();
	}
	else
	{
		Object->Destroy();
	}
}
