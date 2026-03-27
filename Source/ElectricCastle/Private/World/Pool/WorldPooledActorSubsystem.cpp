// Copyright Alien Shores

#include "World/Pool/WorldPooledActorSubsystem.h"

#include "Actor/Pool/PoolableActor.h"
#include "Actor/Pool/PoolManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "World/Pool/WorldPoolingSettings.h"

AActor* UWorldPooledActorSubsystem::SpawnNew(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!World || !ActorClass)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Unable to spawn new actor"), *GetName())
		return nullptr;
	}

	AActor* Actor = World->SpawnActor<AActor>(ActorClass, FTransform());
	if (!Actor)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] SpawnActorDeferred returned null for %s"), *GetName(), *ActorClass->GetName())
		return nullptr;
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetLifeSpan(0);
	// In SpawnNew, after Actor is created:
	Actor->OnDestroyed.AddDynamic(this, &UWorldPooledActorSubsystem::HandlePooledActorDestroyed);


	if (UPoolManagerComponent* PoolManagerComponent = IPoolableActor::GetPoolManager(Actor))
	{
		PoolManagerComponent->SetSpawnPool(this);
		PoolManagerComponent->HandleReturnedToPool();
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Actor %s did not have a pool manager component. It may not pool properly!"), *GetNameSafe(this), *GetNameSafe(Actor))
		PoolManagerComponent = NewObject<UPoolManagerComponent>(Actor, UPoolManagerComponent::StaticClass());
		PoolManagerComponent->RegisterComponent();
		PoolManagerComponent->SetSpawnPool(this);
	}

	// Register in ActorEntries — this is the single owner of the entry.
	// No pool list pointers are stored; the ActorClass key is used to
	// look up the pool on demand.
	FPooledActorEntry Entry;
	Entry.Actor = Actor;
	Entry.ActorClass = ActorClass;
	ActorEntries.Add(Actor, Entry);
	return Actor;
}

AActor* UWorldPooledActorSubsystem::AcquireDeferred_Implementation(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass)
{
	FActorPool& Pool = Pools.FindOrAdd(ActorClass);

	AActor* Actor = nullptr;

	if (Pool.Available.Num() > 0)
	{
		Actor = Pool.Available.Pop();
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Not enough %s in the pool. Adding more..."), *GetNameSafe(this), *ActorClass->GetName())
		Actor = SpawnNew(WorldContextObject, ActorClass);
	}

	if (!Actor)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to get actor of type %s from pool!"), *GetNameSafe(this), *ActorClass->GetName())
		return nullptr;
	}

	Pool.InUse.Add(Actor);

	if (UPoolManagerComponent* PoolManagerComponent = IPoolableActor::GetPoolManager(Actor))
	{
		PoolManagerComponent->HandleBeginRetrieveFromPool();
	}

	return Actor;
}

void UWorldPooledActorSubsystem::AcquireFinalize_Implementation(AActor* Actor, const FTransform& SpawnTransform)
{
	if (!Actor)
	{
		return;
	}

	FPooledActorEntry* Entry = ActorEntries.Find(Actor);
	if (!Entry)
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s] Failed to find pooled entry for actor %s"),
			*GetName(),
			*Actor->GetName()
		);
		return;
	}

	Actor->SetActorTransform(SpawnTransform);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);

	if (UPoolManagerComponent* PoolManagerComponent = IPoolableActor::GetPoolManager(Actor))
	{
		PoolManagerComponent->HandleFinishRetrieveFromPool();
	}
}

void UWorldPooledActorSubsystem::ReturnToPool_Implementation(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	FPooledActorEntry* Entry = ActorEntries.Find(Actor);
	if (!Entry)
	{
		return;
	}

	FActorPool* Pool = Pools.Find(Entry->ActorClass);
	if (!Pool)
	{
		return;
	}

	const int32 Index = Pool->InUse.IndexOfByKey(Actor);
	if (Index != INDEX_NONE)
	{
		Pool->InUse.RemoveAt(Index);
		Pool->Available.Add(Actor);

		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);

		if (UPoolManagerComponent* PoolManagerComponent = IPoolableActor::GetPoolManager(Actor))
		{
			PoolManagerComponent->HandleReturnedToPool();
		}
	}
}

void UWorldPooledActorSubsystem::WarmupPool(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, int32 Count)
{
	FActorPool& Pool = Pools.FindOrAdd(ActorClass);
	Pool.InitialSize = Count;
	Pool.Available.Reserve(Count);

	for (int32 i = 0; i < Count; i++)
	{
		// SpawnNew registers the entry in ActorEntries; place the actor
		// into Available so it is ready for immediate acquisition.
		if (AActor* Actor = SpawnNew(WorldContextObject, ActorClass))
		{
			Pool.Available.Add(Actor);
		}
	}
}

UWorldPooledActorSubsystem* UWorldPooledActorSubsystem::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (const UWorld* World = WorldContextObject->GetWorld())
		{
			return World->GetSubsystem<UWorldPooledActorSubsystem>();
		}
		UE_LOG(LogElectricCastle, Warning, TEXT("[UWorldPooledActorSubsystem::Get] Tried to get world from context object %s, but it was invalid."), *WorldContextObject->GetFName().ToString())
	}
	return nullptr;
}

void UWorldPooledActorSubsystem::OnWorldBeginPlay(UWorld& World)
{
	Super::OnWorldBeginPlay(World);
	// Only warm up in the real PIE/game world
	if (!World.IsGameWorld())
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Skipping warmup: not a game world (%s)"),
			*GetName(),
			*World.GetName()
		);
		return;
	}
	if (const UWorldPoolingSettings* Settings = GetDefault<UWorldPoolingSettings>())
	{
		for (const auto& [ActorClass, PreloadSize] : Settings->WarmUpConfigs)
		{
			WarmupPool(&World, ActorClass, PreloadSize);
		}
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] No world pooling settings!"), *GetName())
	}
}

void UWorldPooledActorSubsystem::OnWorldEndPlay(UWorld& World)
{
	for (auto& Pair : Pools)
	{
		FActorPool& Pool = Pair.Value;

		for (TObjectPtr<AActor>& Actor : Pool.Available)
		{
			if (IsValid(Actor) && !Actor->IsPendingKillPending())
			{
				Actor->OnDestroyed.RemoveAll(this);
				Actor->Destroy();
			}
		}

		for (TObjectPtr<AActor>& Actor : Pool.InUse)
		{
			if (IsValid(Actor) && !Actor->IsPendingKillPending())
			{
				Actor->OnDestroyed.RemoveAll(this);
				Actor->Destroy();
			}
		}
	}

	Pools.Empty();
	ActorEntries.Empty();
}

void UWorldPooledActorSubsystem::HandlePooledActorDestroyed(AActor* DestroyedActor)
{
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[%s] Pooled actor destroyed: %s (World=%s)"),
		*GetNameSafe(this),
		*GetNameSafe(DestroyedActor),
		*GetNameSafe(DestroyedActor ? DestroyedActor->GetWorld() : nullptr)
	);
}
