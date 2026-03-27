#pragma once

#include "CoreMinimal.h"
#include "SpawnPoolTypes.generated.h"

USTRUCT()
struct FPooledActorEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Actor = nullptr;

	// Store the class key so we can look up the owning FActorPool via
	// Pools.Find(ActorClass) on demand. A raw FActorPool* is not safe
	// here because TMap reallocates when new classes are added to Pools,
	// which would leave the pointer dangling.
	UPROPERTY()
	TSubclassOf<AActor> ActorClass = nullptr;

	bool operator==(const FPooledActorEntry& Other) const
	{
		return Actor == Other.Actor;
	}
};


USTRUCT()
struct FActorPool
{
	GENERATED_BODY()

	// Store only actor references here. FPooledActorEntry is owned
	// exclusively by ActorEntries on the subsystem, eliminating all
	// raw pointers into these arrays.
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Available;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> InUse;

	int32 InitialSize = 0;
};
