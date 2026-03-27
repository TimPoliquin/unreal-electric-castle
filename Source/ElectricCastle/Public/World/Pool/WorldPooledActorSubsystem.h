// WorldPooledActorSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Actor/Pool/SpawnPoolInterface.h"
#include "Actor/Pool/SpawnPoolTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldPooledActorSubsystem.generated.h"

UCLASS()
class ELECTRICCASTLE_API UWorldPooledActorSubsystem : public UWorldSubsystem, public ISpawnPoolInterface
{
	GENERATED_BODY()

public:
	virtual AActor* AcquireDeferred_Implementation(const UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass) override;
	virtual void AcquireFinalize_Implementation(AActor* Actor, const FTransform& SpawnTransform) override;
	virtual void ReturnToPool_Implementation(AActor* Actor) override;

	void WarmupPool(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, int32 Count);

	static UWorldPooledActorSubsystem* Get(const UObject* WorldContextObject);

protected:
	virtual void OnWorldBeginPlay(UWorld& World) override;
	virtual void OnWorldEndPlay(UWorld& World) override;

private:
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorPool> Pools;

	// Single source of truth for all entries. Owned by value here so
	// there are no raw pointers into the Available/InUse arrays above.
	// TObjectPtr as the key keeps actors GC-reachable via this map.
	UPROPERTY()
	TMap<TObjectPtr<AActor>, FPooledActorEntry> ActorEntries;

	UFUNCTION()
	void HandlePooledActorDestroyed(AActor* DestroyedActor);
	// Returns the spawned actor. The entry is added to ActorEntries
	// internally; the caller is responsible for placing the actor into
	// the appropriate pool list.
	AActor* SpawnNew(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);
};
