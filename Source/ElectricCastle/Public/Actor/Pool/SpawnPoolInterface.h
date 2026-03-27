// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpawnPoolInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USpawnPoolInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISpawnPoolInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Acquire a deferred actor from the pool
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* AcquireDeferred(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);

	// Finalize the actor (FinishSpawningActor)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AcquireFinalize(AActor* Actor, const FTransform& SpawnTransform);

	// Return actor to pool
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnToPool(AActor* Actor);

	static bool IsSpawnPool(const UObject* Pool);
	static AActor* AcquireDeferred(UObject* Pool, const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);
	static void AcquireFinalize(UObject* Pool, AActor* Actor, const FTransform& SpawnTransform);
	static void ReturnToPool(UObject* Pool, AActor* Actor);
};
