// PoolActorFunctionLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PoolActorFunctionLibrary.generated.h"

UCLASS()
class ELECTRICCASTLE_API UPoolActorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves an actor from the pool without fully instantiating it in the world.
	 * The returned actor is typed to ActorClass, allowing the designer to set
	 * properties before finalization — mirroring BeginDeferredActorSpawnFromClass.
	 *
	 * Call FinalizePooledActor when ready to place it in the world.
	 */
	UFUNCTION(
		BlueprintCallable,
		Category="Pool",
		meta=(
			WorldContext="WorldContextObject",
			DefaultToSelf="WorldContextObject",
			DeterminesOutputType="ActorClass",
			DynamicOutputParam="OutActor",
			DisplayName="Acquire Pooled Actor (Deferred)"
		)
	)
	static void AcquirePooledActorDeferred(
		UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		AActor*& OutActor // Typed to ActorClass by the Blueprint compiler.
		// Hold this reference, set your properties, then
		// pass it to FinalizePooledActor.
	);

	/**
	 * Fully instantiates the pooled actor in the world at the given transform.
	 * Mirrors FinishSpawningActor — call this once all deferred properties are set.
	 */
	UFUNCTION(
		BlueprintCallable,
		Category="Pool",
		meta=(
			WorldContext="WorldContextObject",
			DefaultToSelf="WorldContextObject",
			DisplayName="Finalize Pooled Actor"
		)
	)
	static void FinalizePooledActor(
		UObject* WorldContextObject,
		AActor* Actor,
		FTransform SpawnTransform
	);
};
