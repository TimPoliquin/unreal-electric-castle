// Copyright Alien Shores


#include "Actor/Pool/PoolActorFunctionLibrary.h"
#include "World/Pool/WorldPooledActorSubsystem.h"

void UPoolActorFunctionLibrary::AcquirePooledActorDeferred(
	UObject* WorldContextObject,
	TSubclassOf<AActor> ActorClass,
	AActor*& OutActor
)
{
	OutActor = nullptr;

	if (!WorldContextObject || !ActorClass)
	{
		return;
	}

	UWorldPooledActorSubsystem* Pool = UWorldPooledActorSubsystem::Get(WorldContextObject);
	if (!Pool)
	{
		return;
	}

	OutActor = ISpawnPoolInterface::AcquireDeferred(Pool, WorldContextObject, ActorClass);
}

void UPoolActorFunctionLibrary::FinalizePooledActor(
	UObject* WorldContextObject,
	AActor* Actor,
	FTransform SpawnTransform
)
{
	if (!WorldContextObject || !Actor)
	{
		return;
	}

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!World)
	{
		return;
	}

	UWorldPooledActorSubsystem* Pool = World->GetSubsystem<UWorldPooledActorSubsystem>();
	if (!Pool)
	{
		return;
	}

	ISpawnPoolInterface::AcquireFinalize(Pool, Actor, SpawnTransform);
}
